/**
    part of phrasit

    \author stg7

    \brief key value store wrapper module

    \date 16.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef KVS_HEADER_HPP_
#define KVS_HEADER_HPP_

#include <string>
#include <vector>

#include "leveldb/db.h"

#include "compress/string.hpp"
#include "utils/log.hpp"
#include "utils/helper.hpp"
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
                options.write_buffer_size = 100 * 1024 * 1024;  // 100 MB
                options.max_open_files = 100;
                options.create_if_missing = true;
                leveldb::DB::Open(options, path, db);
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
