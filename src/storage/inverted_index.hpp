/**
    part of phrasit

    \author stg7

    \brief inverted index module

    \date 16.11.2015

    Copyright 2016 Steve Göring

    This file is part of PhrasIt.

    PhrasIt is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PhrasIt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PhrasIt.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef INVERTED_INDEX_HEADER_HPP_
#define INVERTED_INDEX_HEADER_HPP_

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "compress/file.hpp"
#include "sort/external_sort.hpp"
#include "storage/kvs.hpp"
#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "utils/mem.hpp"
#include "utils/progress_bar.hpp"
#include "utils/mmfiles.hpp"
#include "consts.hpp"

namespace phrasit {
    namespace storage {

        class Inverted_index {
         private:
            static constexpr const char* _max_id_key = "__max_id";  //< used as max_id key in kv-store

            static constexpr int MAX_ID_WITDH_BASE_16 = 8;

            static constexpr const char* _tmp_filename = "/_tmp.gz";

            std::string _storagedir;
            unsigned long _max_id;

            kvs::type _meta;

            compress::File<compress::mode::write> _tmpfile;

            phrasit::utils::MMArray<unsigned long> _index;
            phrasit::utils::MMArray<unsigned char> _index_n_and_pos;
            phrasit::utils::MMArray<unsigned long> _index_header;

            std::vector<unsigned long> _ids;
            std::vector<unsigned long> _pos;

            /*
            *
            */
            void fill_header_vectors() {
                phrasit::utils::Progress_bar pb(1000, "read index");

                for (unsigned long l = 0; l < _index_header.size() - 2; l += 2) {
                    _ids.emplace_back(_index_header[l]);
                    _pos.emplace_back(_index_header[l + 1]);
                    pb.update();
                }
                _pos.emplace_back(_index.size() - 1);  //< add dummy positionat the end
            }

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

                // try to restore max id count
                if (_max_id == 0) {
                    _max_id = kvs::count_of_keys(_meta);
                }

                LOGINFO("initialize ii with max_id: " << _max_id);

                bool index_exists = false;
                if (fs::exists(_storagedir + "/" + "_index")) {
                    _index.open(_storagedir + "/" + "_index");
                    index_exists = true;
                }
                if (fs::exists(_storagedir + "/" + "_index_n_and_pos")) {
                    _index_n_and_pos.open(_storagedir + "/" + "_index_n_and_pos");
                    index_exists = true;
                }

                if (fs::exists(_storagedir + "/" + "_index_header")) {
                    _index_header.open(_storagedir + "/" + "_index_header");
                    index_exists = true;
                    // copy index header content to memory, for better performance and
                    //  because of the fact, that the header is quite small
                    //  (just for each 1gram id one unsigned long for the start position)
                    fill_header_vectors();
                }

                if (!index_exists) {
                    _tmpfile.open(_storagedir + _tmp_filename);
                }
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
                phrasit::utils::check(_index.is_open() == false, "index file exists, you cannot insert new values to an existing creted index");

                unsigned long id = kvs::get_ulong_or_default(_meta, ngram_token, _max_id);
                if (id == _max_id) {
                    kvs::put(_meta, ngram_token, std::to_string(_max_id));
                    _max_id++;
                }

                // TODO(stg7) there is a better approach, not using leading zeros for correct sorting
                //  instead use a binary format and store triples, but external sort must be modified
                std::ostringstream line;
                line << std::setw(MAX_ID_WITDH_BASE_16)
                    << std::setfill('0') << std::hex << id << "\t"
                    << std::setw(2) << std::setfill('0') << std::hex << (10 * pos + n) << "\t"
                    << std::setw(16) << std::setfill('0') << std::hex << ngram_id;

                _tmpfile.writeLine(line.str());
                return true;
            }

            /*
            *   optimize index: build a memory mapped index
            *   if ignore_existing is true then an already existing index will be ignored
            */
            inline bool optimize(bool ignore_existing = false) {
                namespace fs = boost::filesystem;

                std::string tmp_filename = _storagedir + _tmp_filename;

                if (!fs::exists(tmp_filename) && !ignore_existing) {
                    LOGERROR("index is optimized, or something is wrong with file: " << tmp_filename);
                    return false;
                }

                if (_tmpfile.is_open()) {
                    _tmpfile.close();
                }



                long blocksize = utils::memory::get_free() * 1024 / 4;

                LOGINFO("using an estimated blocksize of " << utils::size::to_mb(blocksize) << " mb");

                std::string resultfilename = sort::external_sort(tmp_filename, _storagedir, blocksize);

                // if tmp file is opened in append mode, don't delete tmp
                if (fs::exists(tmp_filename)) {
                    // fs::remove(tmp_filename);
                }

                fs::rename(resultfilename, _storagedir + "/" + "_sorted.gz");
                resultfilename = _storagedir + "/" + "_sorted.gz";
                LOGINFO("sorted file " << resultfilename);

                LOGINFO("build index");

                unsigned long line_count = phrasit::utils::count_lines_compressed(resultfilename);

                LOGINFO("count of lines: " << line_count);

                compress::File<compress::mode::read> index_txt_file(resultfilename);

                // TODO(stg7) maybe dont use unsigned long as index value, e.g. struct type

                // store values in binary format using mmfiles -> size = count_of_lines(resultfilename) * 3 * size_of(ulong)
                _index.open(_storagedir + "/" + "_index", line_count * sizeof(unsigned long));
                _index_n_and_pos.open(_storagedir + "/" + "_index_n_and_pos", line_count * sizeof(unsigned char));
                // store start positions in header file
                _index_header.open(_storagedir + "/" + "_index_header", (_max_id + 1) * 2 * sizeof(unsigned long));

                char delimiter = '\t';
                unsigned long current_id = 0;

                unsigned long pos = 0;
                unsigned long h_pos = 0;
                _index_header[h_pos++] = current_id;
                _index_header[h_pos++] = 0;

                phrasit::utils::Progress_bar pb(1000, "index");

                for(std::string line = ""; index_txt_file.readLine(line);) {

                    std::vector<std::string> splitted_line = phrasit::utils::split(line, delimiter);

                    if (splitted_line.size() == 3) {

                        unsigned long id = std::stol(splitted_line[0], nullptr, 16);
                        unsigned char n_and_pos = std::stoi(splitted_line[1], nullptr, 16);
                        unsigned long ngram_id = std::stol(splitted_line[2], nullptr, 16);

                        if (id != current_id) {
                            _index_header[h_pos++] = id;
                            _index_header[h_pos++] = pos;
                            current_id = id;
                        }

                        _index_n_and_pos[pos] = n_and_pos;
                        _index[pos] = ngram_id;
                        pos ++;
                        pb.update();
                    }
                }

                // store dummy element at the end of header for easy accessing
                _index_header[h_pos++] = 0;
                _index_header[h_pos++] = _index.size() - 1;

                if (phrasit::debug) {  // write validation file, for debugging
                    compress::File<compress::mode::write> validation_file;

                    validation_file.open(_storagedir + "/_validation.gz");
                    for (unsigned long l = 0; l < _index_header.size() - 2; l += 2) {
                        unsigned long id = _index_header[l];
                        unsigned long pos = _index_header[l + 1];
                        unsigned long next_pos = _index_header[l + 3];

                        for (unsigned long j = pos; j < next_pos; j ++) {
                            unsigned long n_and_pos = _index_n_and_pos[j];
                            unsigned long ngram_id = _index[j];
                            std::ostringstream v_line;
                            v_line << std::setw(MAX_ID_WITDH_BASE_16)
                                << std::setfill('0') << std::hex << id << "\t"
                                << std::setw(2) << std::setfill('0') << std::hex << n_and_pos << "\t"
                                << std::setw(16) << std::setfill('0') << std::hex << ngram_id;

                            validation_file.writeLine(v_line.str());
                        }
                    }
                }
                if (!phrasit::debug) {
                    fs::remove(resultfilename);
                    //fs::remove(tmp_filename); // don't delete tmp file, for later appending mode
                }

                fill_header_vectors();
                return true;
            }

            std::vector<unsigned long> get_by_key(const std::string& key, unsigned long needed_n = 0, unsigned long needed_pos = 0) {
                phrasit::utils::check(_index.is_open() == true, "index file is closed?!");

                std::vector<unsigned long> res;
                unsigned char needed_n_and_pos = (char) (10 * needed_pos + needed_n);

                // check if key is stored
                unsigned long key_id = kvs::get_ulong_or_default(_meta, key, _max_id);
                if (_max_id == key_id) {
                    return res;
                }

                // perform a binary search in the header to get start and end positions of the index
                auto header_pos = std::lower_bound(_ids.begin(), _ids.end(), key_id) - _ids.begin();
                unsigned long start_pos = _pos[header_pos];
                unsigned long end_pos = _pos[header_pos + 1];

                // get all suitable n-grams, the resulting vector is sorted, because the index
                //  is sorted
                for (unsigned long j = start_pos; j < end_pos; j ++) {
                    unsigned char n_and_pos = _index_n_and_pos[j];
                    if (needed_n_and_pos == 0 || n_and_pos == needed_n_and_pos) {
                        unsigned long ngram_id = _index[j];
                        res.emplace_back(ngram_id);
                    }
                }
                return res;
            }
        };
    }
}
#endif
