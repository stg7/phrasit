/**
    part of phrasit

    \author stg7

    \brief Phrasit module

    \date 15.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef PHRASIT_HEADER_HPP_
#define PHRASIT_HEADER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <queue>
#include <tuple>
#include <algorithm>

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
            print_stats(std::cout);

            LOGINFO("delete store");
            close();
        }

        inline void close() {
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
            std::vector<std::string> parts = phrasit::utils::filter(
                phrasit::utils::split(phrasit::utils::trim(ngram), ' '), phrasit::notempty_filter);

            std::string cleaned_ngram = phrasit::utils::join(parts, " ");

            unsigned long n = parts.size();
            phrasit::utils::check(n <= phrasit::max_ngram, "ngram size > " + std::to_string(phrasit::max_ngram));

            long id = get_id_by_ngram(cleaned_ngram);
            if (id == -1) {
                id = _max_id++;

                std::string id_str = std::to_string(id);

                storage::kvs::put(_ngram_to_id, cleaned_ngram, id_str);
                storage::kvs::put(_id_to_ngram, id_str, cleaned_ngram);
                storage::kvs::put(_freq, id_str, freq);

                // update global ngram statistic
                long xgram_count = storage::kvs::get_ulong_or_default(_global_statistic, std::to_string(n), 0);
                xgram_count++;

                storage::kvs::put(_global_statistic, std::to_string(n), std::to_string(xgram_count));

                //std::cout << "current id: " << id << std::endl;
                int pos = 1;
                for (auto& x : parts) {
                    //std::cout << x << " __ " << std::endl;
                    _index->append(x, id, n, pos);
                    pos++;
                }
                //std::cout << std::endl;
            }
            return id;
        }

        void optimize(bool ignore_existing=false) {
            LOGINFO("optimize");
            //close();
            _index->optimize(ignore_existing);
            // TODO(stg7)
        }

        std::string get_ngram(const unsigned long id) {
            std::string ngram = "";
            storage::kvs::get(_id_to_ngram, std::to_string(id), &ngram);
            return ngram;
        }

        unsigned long get_freq(const unsigned long id) {
            return storage::kvs::get_ulong_or_default(_freq, std::to_string(id), 0);
        }

        /*
        *   handle a query and return all results as a vector
        */
        const std::vector<unsigned long> search(const std::string& query) {
            std::vector<unsigned long> res;

            // TODO(stg7) add a real query parser

            std::vector<std::string> parts = phrasit::utils::filter(
                phrasit::utils::split(phrasit::utils::trim(query), ' '), phrasit::notempty_filter);

            std::string cleaned_query = phrasit::utils::join(parts, " ");

            LOGINFO("handle query: " << query);

            std::map<unsigned long, unsigned long> res_map;

            int pos = 1;
            unsigned long query_parts = parts.size();

            std::vector<unsigned long> result_ids;

            // get all results and store it in a map, maybe TODO(stg7) use a set
            // because this approach is an intersection of all result sets for each part
            for (auto& x : parts) {

                if (x == "?") {
                    query_parts --;
                    pos ++;
                    continue;
                }

                for (auto& y : _index->get_by_key(x, parts.size(), pos)) {
                    if (res_map.find(y) != res_map.end()) {
                        res_map[y] ++;
                    } else {
                        res_map[y] = 1;
                    }
                }
                pos ++;
            }

            // sort results based on ngram frequency

            typedef std::tuple<unsigned long, unsigned long> pair;

            auto cmp = [](pair& left, pair& right) -> bool {return std::get<1>(left) > std::get<1>(right);};

            std::priority_queue<pair, std::vector<pair>, decltype(cmp) > queue(cmp);


            for (auto& x : res_map) {
                // only use ngrams, that are in the intersection of all parts
                if (x.second == query_parts) {
                    queue.push(std::make_tuple(x.first, get_freq(x.first)));

                    // remove elements from queue to reduce memory overhead
                    // if a query will receive a lot of results
                    if (queue.size() > phrasit::max_result_size) {
                        queue.pop();
                    }
                }
            }

            // insert elements sorted to result vector, from min to max frequency
            while (!queue.empty()) {
                auto top = queue.top();
                res.push_back(std::get<0>(top));
                queue.pop();
            }
            // reverse result, because, most frequent should be first
            // this approach is necessary because of the result size limitation
            std::reverse(std::begin(res), std::end(res));
            return res;
        }

        /*
        *   printout global collected statistics
        */
        const void print_stats(std::ostream& out) {
            out << "phrasit statistics:" << std::endl;
            std::string xgram_count = "";
            for(int n = 1; n < phrasit::max_ngram + 1; n++) {
                if (!storage::kvs::get(_global_statistic, std::to_string(n), &xgram_count)) {
                    xgram_count = "0";
                }
                out << "n= " << n  << " -> " << xgram_count << std::endl;
            }

            out << "max_id: " << _max_id << std::endl;
        }
    };
}
#endif
