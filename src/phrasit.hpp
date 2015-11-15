/**
    part of phrasit

    \author stg7

    \brief store module

    \date 15.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef STORE_HEADER_HPP_
#define STORE_HEADER_HPP_

#include <string>
#include <vector>

#include "leveldb/db.h"

#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "consts.hpp"

namespace phrasit {

    // wrapper for key value store (level db), for better exchange if needed
    template<typename K, typename V, typename DB> inline void kvs_put(DB db, K key, V value) {
        db->Put(leveldb::WriteOptions(), key, value);
    }

    template<typename K, typename V, typename DB> inline bool kvs_get(DB db, K key, V value) {
        leveldb::Status s = db->Get(leveldb::ReadOptions(), key, value);
        if (!s.ok()) {
            return false;
        }
        return true;
    }

    template<typename K, typename DB> inline bool kvs_get_long_or_default(DB db, K key, long def) {
        std::string value = "";
        if (kvs_get(db, key, &value)) {
            return std::stol(value);
        }
        return def;
    }

    template<typename P, typename DB> inline void kvs_open(P path, DB db) {
        leveldb::Options options;
        options.create_if_missing = true;
        leveldb::DB::Open(options, path, db);
    }

    template<typename DB> inline void kvs_close(DB db) {
        delete db;
    }
    // wrapper end

    class Phrasit {
     private:
        static constexpr const char* _max_id_key = "__max_id";

        std::string _storagedir;
        leveldb::DB* _ngram_to_id;
        leveldb::DB* _id_to_ngram;
        leveldb::DB* _freq;  //< maps id to ngram freq
        leveldb::DB* _global_statistic;

        long _max_id;

     public:
        Phrasit(const std::string& storagedir): _storagedir(storagedir), _max_id(0) {
            LOGINFO("create store");

            leveldb::Options options;
            options.create_if_missing = true;
            kvs_open(_storagedir + "/_ngram_to_id", &_ngram_to_id);
            kvs_open(_storagedir + "/_id_to_ngram", &_id_to_ngram);
            kvs_open(_storagedir + "/_freq", &_freq);
            kvs_open(_storagedir + "/_global_statistic", &_global_statistic);


            std::string _max_id_str = "";

            if (!kvs_get(_ngram_to_id, _max_id_key, &_max_id_str)) {
                _max_id = 0;
            } else {
                try {
                    _max_id = std::stol(_max_id_str);
                } catch(...) {
                    _max_id = 0;
                }
            }
            LOGINFO("initialize store with max_id: " << _max_id);
        }

        ~Phrasit() {
            print_stats();

            LOGINFO("delete store");

            kvs_put(_ngram_to_id, _max_id_key, std::to_string(_max_id));

            kvs_close(_ngram_to_id);
            kvs_close(_id_to_ngram);
            kvs_close(_freq);
            kvs_close(_global_statistic);
        }

        long get_id_by_ngram(const std::string& ngram) {
            std::string value = "";
            if (kvs_get(_ngram_to_id, ngram, &value)) {
                return std::stol(value);
            }
            return -1;
        }

        /*
        *   insert ngram with frequency
        */
        long insert(const std::string& ngram, const std::string& freq) {
            std::string cleaned_ngram = phrasit::utils::trim(ngram);
            std::vector<std::string> parts = phrasit::utils::filter(
                phrasit::utils::split(cleaned_ngram, ' '), [](std::string& x){return x != "";});

            cleaned_ngram = phrasit::utils::join(parts, " ");

            phrasit::utils::check(parts.size() <= phrasit::max_ngram, "ngram size > " + std::to_string(phrasit::max_ngram));

            long id = get_id_by_ngram(cleaned_ngram);
            if (id == -1) {
                _max_id++;

                kvs_put(_ngram_to_id, cleaned_ngram, std::to_string(_max_id));
                kvs_put(_id_to_ngram, std::to_string(_max_id), cleaned_ngram);
                kvs_put(_freq, std::to_string(_max_id), freq);

                // update global ngram statistic
                long xgram_count = kvs_get_long_or_default(_global_statistic, std::to_string(parts.size()), 0);
                xgram_count++;

                kvs_put(_global_statistic, std::to_string(parts.size()), std::to_string(xgram_count));

                // TODO(stg7) store parts in (inverted) index
                for (auto& x : parts) {
                    std::cout << x << " __ " << std::endl;
                }
                std::cout << std::endl;

                return _max_id;
            }
            return id;
        }

        /*
        *   handle a query and return all results as a vector
        */
        const std::vector<std::string> search(const std::string& query) {
            std::vector<std::string> res;

            std::string cleaned_query = phrasit::utils::trim(query);
            std::vector<std::string> parts = phrasit::utils::filter(
                phrasit::utils::split(cleaned_query, ' '), [](std::string& x){return x != "";});

            cleaned_query = phrasit::utils::join(parts, " ");
            std::cout << cleaned_query << std::endl;
            for (auto& x : parts) {
                std::cout << x << " __ " << std::endl;
            }
            res.push_back("test");
            return res;
        }

        /*
        *   printout global collected statistics
        */
        const void print_stats() {
            std::cout << "phrasit statistics:" << std::endl;
            std::string xgram_count = "";
            for(int n = 1; n < phrasit::max_ngram + 1; n++) {
                if (!kvs_get(_global_statistic, std::to_string(n), &xgram_count)) {
                    xgram_count = "0";
                }
                std::cout << "n= " << n  << " -> " << xgram_count << std::endl;
            }

            std::cout << "max_id: " << _max_id << std::endl;
        }
    };
}
#endif
