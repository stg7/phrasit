/**
    part of phrasit

    \author stg7

    \brief Phrasit module

    \date 15.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef STORE_HEADER_HPP_
#define STORE_HEADER_HPP_

#include <string>
#include <vector>
#include <memory>

#include "storage/kvs.hpp"
#include "storage/inverted_index.hpp"
#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "consts.hpp"

namespace phrasit {

    class Phrasit {
     private:
        static constexpr const char* _max_id_key = "__max_id";

        std::string _storagedir;
        storage::kvs::type _ngram_to_id;
        storage::kvs::type _id_to_ngram;
        storage::kvs::type _freq;  //< maps id to ngram freqs
        storage::kvs::type _global_statistic;

        std::shared_ptr<storage::Inverted_index> _index = nullptr;

        unsigned long _max_id;

     public:
        Phrasit(const std::string& storagedir): _storagedir(storagedir), _max_id(0) {
            LOGINFO("create store");
            _index = std::shared_ptr<storage::Inverted_index>(new storage::Inverted_index(_storagedir));

            storage::kvs::open(_storagedir + "/_ngram_to_id", &_ngram_to_id);
            storage::kvs::open(_storagedir + "/_id_to_ngram", &_id_to_ngram);
            storage::kvs::open(_storagedir + "/_freq", &_freq);
            storage::kvs::open(_storagedir + "/_global_statistic", &_global_statistic);

            _max_id = storage::kvs::get_ulong_or_default(_ngram_to_id, _max_id_key, 0);

            LOGINFO("initialize store with max_id: " << _max_id);
        }

        ~Phrasit() {
            print_stats();

            LOGINFO("delete store");

            storage::kvs::put(_ngram_to_id, _max_id_key, std::to_string(_max_id));

            storage::kvs::close(_ngram_to_id);
            storage::kvs::close(_id_to_ngram);
            storage::kvs::close(_freq);
            storage::kvs::close(_global_statistic);
        }

        long get_id_by_ngram(const std::string& ngram) {
            std::string value = "";
            if (storage::kvs::get(_ngram_to_id, ngram, &value)) {
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

                storage::kvs::put(_ngram_to_id, cleaned_ngram, std::to_string(_max_id));
                storage::kvs::put(_id_to_ngram, std::to_string(_max_id), cleaned_ngram);
                storage::kvs::put(_freq, std::to_string(_max_id), freq);

                // update global ngram statistic
                long xgram_count = storage::kvs::get_ulong_or_default(_global_statistic, std::to_string(parts.size()), 0);
                xgram_count++;

                storage::kvs::put(_global_statistic, std::to_string(parts.size()), std::to_string(xgram_count));

                // TODO(stg7) store parts in (inverted) index

                unsigned long id = _max_id;
                unsigned long n = parts.size();
                //std::cout << "current id: " << id << std::endl;
                for (auto& x : parts) {
                    //std::cout << x << " __ " << std::endl;
                    _index->append(x, id, n);

                }
                //std::cout << std::endl;

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
                phrasit::utils::split(cleaned_query, ' '), phrasit::notempty_filter);

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
                if (!storage::kvs::get(_global_statistic, std::to_string(n), &xgram_count)) {
                    xgram_count = "0";
                }
                std::cout << "n= " << n  << " -> " << xgram_count << std::endl;
            }

            std::cout << "max_id: " << _max_id << std::endl;
        }
    };
}
#endif
