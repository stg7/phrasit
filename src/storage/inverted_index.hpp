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

                //LOGDEBUG("ngram_token " << ngram_token);
                //LOGDEBUG("id " << id);
                /*
                if (_stores.find(id) == _stores.end()) {
                    kvs::open(_storagedir + "/_" + std::to_string(id), &_stores[id]);
                }
                */
                _tmpfile << id << "\t" << ngram_id << "\t" << n << "\n";

                // easy way:
                // store in tmpfile: (id, ngram, n)
                // sort external by id
                // store all in a file and save startpos/endpos of ids in another file

                //LOGINFO("put: " << ngram_id << " -> " << n);
                //kvs::put(_stores[id], std::to_string(ngram_id), std::to_string(n));
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

                LOGINFO("sorted file " << resultfilename);
                // todo(stg7) rename resultfile

                LOGINFO("build index");

                // store values in binary format using mmfiles -> size = count_of_lines(resultfilename) * 3 * size_of(ulong)
                // store start & end positions in header file
                return true;

            }
        };
    }
}
#endif
