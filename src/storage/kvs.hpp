/**
    part of phrasit

    \author stg7

    \brief key value store wrapper module

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

#ifndef KVS_HEADER_HPP_
#define KVS_HEADER_HPP_

#include <string>
#include <vector>

#include "leveldb/db.h"

#include "compress/string.hpp"
#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "utils/mmfiles.hpp"
#include "consts.hpp"

namespace phrasit {
    namespace storage {
        namespace kvs {
            typedef leveldb::DB* type;

            // wrapper for key value store (level db), for better exchange if needed
            template<typename K, typename V, typename DB> inline void put(DB db, K key, V value) {
                db->Put(leveldb::WriteOptions(), key, value);
            }

            template<typename K, typename V, typename DB> inline bool get(DB db, K key, V value) {
                leveldb::Status s = db->Get(leveldb::ReadOptions(), key, value);
                if (!s.ok()) {
                    return false;
                }
                return true;
            }

            template<typename K, typename DB> inline unsigned long get_ulong_or_default(DB db, K key, unsigned long def) {
                std::string value = "";
                if (get(db, key, &value)) {
                    return std::stol(value);
                }
                return def;
            }

            template<typename P, typename DB> inline void open(P path, DB db) {
                leveldb::Options options;
                options.write_buffer_size = utils::size::mb(400);
                options.max_open_files = 100;
                options.block_size = 4000 * 10;
                /*
                options.block_restart_interval = 16 * 10;
                */
                options.create_if_missing = true;
                leveldb::DB::Open(options, path, db);
            }

            template<typename DB> unsigned long count_of_keys(DB db) {
                leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
                unsigned long count = 0;
                for (it->SeekToFirst(); it->Valid(); it->Next()) {
                    count++;
                }
                assert(it->status().ok());  // Check for any errors found during the scan
                delete it;
                return count;
            }

            template<typename DB> inline void close(DB db) {
                delete db;
                db = nullptr;
            }
            // wrapper end
        }
    }
}
#endif
