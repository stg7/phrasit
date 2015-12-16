/**
    part of phrasit

    \author stg7

    \brief inverted index module

    \date 16.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef INVERTED_INDEX_HEADER_HPP_
#define INVERTED_INDEX_HEADER_HPP_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "boost/filesystem.hpp"

#include "sort/external_sort.hpp"
#include "storage/kvs.hpp"
#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "utils/progress_bar.hpp"
#include "utils/mmfiles.hpp"
#include "consts.hpp"

namespace phrasit {
    namespace storage {

        class Inverted_index {
         private:
            static constexpr const char* _max_id_key = "__max_id";  //< used as max_id key in kv-store

            std::string _storagedir;
            unsigned long _max_id;

            kvs::type _meta;
            std::ofstream _tmpfile;

            phrasit::utils::MMArray<unsigned long> _index;
            phrasit::utils::MMArray<unsigned long> _index_header;

            std::vector<unsigned long> _ids;
            std::vector<unsigned long> _pos;


         public:
            Inverted_index(const std::string& storagedir) : _max_id(0) {
                LOGINFO("create ii");
                _storagedir = storagedir + "/_ii";
                namespace fs = boost::filesystem;

                if (!fs::exists(_storagedir)) {
                    LOGMSG("create storagedir for ii: " << _storagedir);
                    fs::create_directory(_storagedir);
                }

                kvs::open(_storagedir + "/_meta", &_meta);
                _max_id = kvs::get_ulong_or_default(_meta, _max_id_key, 0);


                bool index_exists = false;
                if (fs::exists(_storagedir + "/" + "_index")) {
                    _index.open(_storagedir + "/" + "_index");
                    index_exists = true;
                }
                if (fs::exists(_storagedir + "/" + "_index_header")) {
                    _index_header.open(_storagedir + "/" + "_index_header");
                    index_exists = true;
                }

                if (!index_exists) {
                    _tmpfile.open(_storagedir + "/_tmp", std::ofstream::out | std::ofstream::app);
                    _tmpfile.sync_with_stdio(false);
                }

                // copy index header content to memory, for better performance and
                //  because of the fact, that the header is quite small
                //  (just for each 1gram id one unsigned long for the start position)
                for (unsigned long l = 0; l < _index_header.size() - 2; l += 2) {
                    _ids.emplace_back(_index_header[l]);
                    _pos.emplace_back(_index_header[l + 1]);
                }
                // TODO(stg7) index header can be closed

            }

            ~Inverted_index() {
                kvs::put(_meta, _max_id_key, std::to_string(_max_id));

                if (_tmpfile.is_open()) {
                    _tmpfile.close();
                }

                kvs::close(_meta);
                LOGINFO("delete ii");
            }

            /*
            *   append ngram token with ngram id and "n" to the index
            */
            inline bool append(const std::string& ngram_token, unsigned long ngram_id, unsigned long n, unsigned long pos) {
                phrasit::utils::check(_index.is_open() == false, "index file is there, you cannot insert new values to an existing creted index");
                unsigned long id = kvs::get_ulong_or_default(_meta, ngram_token, _max_id);
                if (id == _max_id) {
                    kvs::put(_meta, ngram_token, std::to_string(_max_id));
                    _max_id++;
                }

                _tmpfile << id << "\t" << ngram_id << "\t" << (10 * pos + n) << "\n";

                return true;
            }

            /*
            *   optimize index: build a memory mapped index
            *   if ignore_existing is true then an already existing index will be ignored
            */
            inline bool optimize(bool ignore_existing=false) {
                namespace fs = boost::filesystem;

                std::string tmp_filename = _storagedir + "/_tmp";

                if (!fs::exists(tmp_filename) && !ignore_existing) {
                    LOGERROR("index is optimized, or something is wrong with file: " << tmp_filename);
                    return false;
                }

                std::string resultfilename = sort::external_sort(tmp_filename, _storagedir);

                // it tmp file is opened in append mode, don't delete tmp
                if (fs::exists(tmp_filename)) {
                    //fs::remove(tmp_filename);
                }

                fs::rename(resultfilename, _storagedir + "/" + "_sorted");
                resultfilename = _storagedir + "/" + "_sorted";
                LOGINFO("sorted file " << resultfilename);

                LOGINFO("build index");

                std::ifstream index_txt_file(resultfilename);

                std::string line = "";
                unsigned long line_count = phrasit::utils::count_lines(resultfilename);
                LOGINFO("count of lines: " << line_count);

                // TODO(stg7) maybe dont use unsigned long as index value, e.g. struct type

                // store values in binary format using mmfiles -> size = count_of_lines(resultfilename) * 3 * size_of(ulong)
                _index.open(_storagedir + "/" + "_index", line_count * 2 * sizeof(unsigned long));
                // store start positions in header file
                _index_header.open(_storagedir + "/" + "_index_header", (_max_id + 1) * 2 * sizeof(unsigned long));

                char delimiter = '\t';
                unsigned long current_id = 0;

                unsigned long pos = 0;
                unsigned long h_pos = 0;
                _index_header[h_pos++] = current_id;
                _index_header[h_pos++] = 0;


                phrasit::utils::Progress_bar pb(1000, "index");
                while (!index_txt_file.eof()) {
                    getline(index_txt_file, line);

                    std::vector<std::string> splitted_line = phrasit::utils::split(line, delimiter);

                    if (splitted_line.size() == 3) {
                        unsigned long id = std::stol(splitted_line[0]);
                        unsigned long ngram_id = std::stol(splitted_line[1]);
                        unsigned long n_and_pos = std::stol(splitted_line[2]);

                        if (id != current_id) {
                            _index_header[h_pos++] = id;
                            _index_header[h_pos++] = pos;
                            current_id = id;
                        }

                        _index[pos++] = ngram_id;
                        _index[pos++] = n_and_pos;
                        pb.update();
                    }
                }

                // store dummy element at the end of header for easy accessing
                _index_header[h_pos++] = 0;
                _index_header[h_pos++] = _index.size() - 1;

                { // write validation file, for debugging
                    std::ofstream validation_file;
                    validation_file.open(_storagedir + "/_validation");
                    for (unsigned long l = 0; l < _index_header.size() - 2; l += 2) {
                        unsigned long id = _index_header[l];
                        unsigned long pos = _index_header[l + 1];
                        //unsigned long next_id = _index_header[l + 2];
                        unsigned long next_pos = _index_header[l + 3];

                        for (unsigned long j = pos; j < next_pos; j += 2) {
                            unsigned long ngram_id = _index[j];
                            unsigned long n_and_pos = _index[j + 1];
                            validation_file << id << "\t" << ngram_id << "\t" << n_and_pos << "\n";
                        }
                    }
                }
                return true;
            }

            std::vector<unsigned long> get_by_key(const std::string& key, unsigned long needed_n = 0, unsigned long needed_pos = 0) {
                phrasit::utils::check(_index.is_open() == true, "index file is closed?!");

                std::vector<unsigned long> res;
                unsigned long needed_n_and_pos = (10 * needed_pos + needed_n);

                // check if key is stored
                unsigned long key_id = kvs::get_ulong_or_default(_meta, key, _max_id);
                if (_max_id == key_id) {
                    return res;
                }

                unsigned long start_pos = 0;
                unsigned long end_pos = 0;

                for (unsigned long l = 0; l < _ids.size(); l++) {
                    if (_ids[l] == key_id) {
                        start_pos = _pos[l];
                        end_pos = _pos[l+1];
                        break;
                    }
                }

                for (unsigned long j = start_pos; j < end_pos; j += 2) {
                    unsigned long ngram_id = _index[j];
                    unsigned long n_and_pos = _index[j + 1];
                    if (needed_n_and_pos == 0 || needed_n_and_pos == n_and_pos) {
                        res.emplace_back(ngram_id);
                    }
                }
                return res;
            }
        };
    }
}
#endif
