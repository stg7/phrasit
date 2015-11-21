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
            static constexpr const char* _max_id_key = "__max_id";

            std::string _storagedir;
            unsigned long _max_id;

            kvs::type _meta;

            std::ofstream _tmpfile;

            std::map<unsigned long, kvs::type> _stores;


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

                // TODO(stg7) open it in append mode
                _tmpfile.open(_storagedir + "/_tmp");

                std::cout << "keys" << std::endl;
                leveldb::Iterator* it = _meta->NewIterator(leveldb::ReadOptions());
                long i = 0;
                for (it->SeekToFirst(); it->Valid(); it->Next()) {
                    std::cout << it->key().ToString() << ": "  << it->value().ToString() << std::endl;
                    i++;
                    if (i > 100) {
                        break;
                    }
                }

            }

            ~Inverted_index() {
                kvs::put(_meta, _max_id_key, std::to_string(_max_id));
                for(auto& k : _stores) {
                    kvs::close(k.second);
                }

                if (_tmpfile.is_open()) {
                    _tmpfile.close();
                }

                kvs::close(_meta);
                LOGINFO("delete ii");
            }

            inline bool append(const std::string& ngram_token, unsigned long ngram_id, unsigned long n) {
                //LOGINFO("append(" << ngram_token << ", " << ngram_id << ")");

                unsigned long id = kvs::get_ulong_or_default(_meta, ngram_token, _max_id);
                if (id == _max_id) {
                    kvs::put(_meta, ngram_token, std::to_string(_max_id));
                    _max_id++;
                }

                _tmpfile << id << "\t" << ngram_id << "\t" << n << "\n";

                return true;
            }

            inline bool optimize() {
                namespace fs = boost::filesystem;

                std::string tmp_filename = _storagedir + "/_tmp";

                if (!fs::exists(tmp_filename)) {
                    LOGERROR("index is optimized, or something wrong with file: " << tmp_filename);
                    return false;
                }

                if (_tmpfile.is_open()) {
                    _tmpfile.close();
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

                // store values in binary format using mmfiles -> size = count_of_lines(resultfilename) * 3 * size_of(ulong)
                phrasit::utils::MMArray<unsigned long> index(_storagedir + "/" + "_index", line_count * 2 * sizeof(unsigned long));
                // store start positions in header file
                phrasit::utils::MMArray<unsigned long> index_header(_storagedir + "/" + "_index_header", (_max_id + 1) * 2 * sizeof(unsigned long));

                char delimiter = '\t';
                unsigned long current_id = 0;

                unsigned long pos = 0;
                unsigned long h_pos = 0;
                index_header[h_pos] = current_id;
                h_pos++;
                index_header[h_pos] = 0;
                h_pos++;


                phrasit::utils::Progress_bar pb(100);
                while (!index_txt_file.eof()) {
                    getline(index_txt_file, line);

                    std::vector<std::string> splitted_line = phrasit::utils::split(line, delimiter);

                    if (splitted_line.size() == 3) {
                        unsigned long id = std::stol(splitted_line[0]);
                        unsigned long ngram_id = std::stol(splitted_line[1]);
                        unsigned long n = std::stol(splitted_line[2]);

                        if (id != current_id) {
                            index_header[h_pos] = id;
                            h_pos++;
                            index_header[h_pos] = pos;
                            h_pos++;
                            current_id = id;
                        }

                        index[pos] = ngram_id;
                        pos++;
                        index[pos] = n;
                        pos ++;
                        pb.update();
                    }
                }
                std::cout << std::endl;

                // store dummy element at the end of header for easy accessing
                index_header[h_pos] = 0;
                h_pos++;
                index_header[h_pos] = index.size() - 2;
                h_pos++;

                {
                    std::ofstream validation_file;
                    validation_file.open(_storagedir + "/_validation");
                    for (unsigned long l = 0; l < index_header.size() - 2; l += 2) {
                        unsigned long id = index_header[l];
                        unsigned long pos = index_header[l + 1];
                        //unsigned long next_id = index_header[l + 2];
                        unsigned long next_pos = index_header[l + 3];

                        for (unsigned long j = pos; j < next_pos; j += 2) {
                            unsigned long ngram_id = index[j];
                            unsigned long n = index[j+1];
                            validation_file << id << "\t" << ngram_id << "\t" << n << "\n";
                        }
                    }
                }
                return true;
            }
        };
    }
}
#endif
