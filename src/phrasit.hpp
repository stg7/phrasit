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

#include "consts.hpp"
#include "parser/query_parser.hpp"
#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "storage/kvs.hpp"
#include "storage/inverted_index.hpp"

namespace phrasit {

    class Phrasit {
     private:
        static constexpr const char* _max_id_key = "__max_id";

        std::string _storagedir;
        storage::kvs::type _ngram_to_id;
        storage::kvs::type _id_to_ngram;
        storage::kvs::type _freq;  //< maps id to n-gram frequencies
        storage::kvs::type _global_statistic;

        std::shared_ptr<storage::Inverted_index> _index = nullptr;

        unsigned long _max_id;
        parser::Query_parser _parser;

        // sort results based on n-gram frequency
        const inline std::vector<unsigned long> sort_ngram_ids_by_freq(const std::vector<unsigned long>& result_ids) {
            std::vector<unsigned long> res;
            //typedef std::tuple<unsigned long, unsigned long> pair;
            // TODO(stg7) is it possible to remove the tuple struct and use direct the get_freq call?
            auto cmp = [this](unsigned long& left, unsigned long& right) -> bool {
                return get_freq(left) > get_freq(right);
            };

            //std::priority_queue<pair, std::vector<pair>, decltype(cmp) > queue(cmp);
            std::priority_queue<unsigned long, std::vector<unsigned long>, decltype(cmp) > queue(cmp);

            for (auto& x : result_ids) {
                queue.push(x);

                // remove elements from queue to reduce memory overhead
                //  if a query will receive a lot of results
                if (queue.size() > phrasit::max_result_size) {
                    queue.pop();
                }
            }

            // insert elements sorted to result vector, from min to max frequency
            while (!queue.empty()) {
                auto top = queue.top();
                res.emplace_back(top);
                queue.pop();
            }
            // reverse results, because, most frequent should be first
            //  this approach is necessary because of the result size
            //  limitation using the priority queue
            std::reverse(std::begin(res), std::end(res));
            return res;
        }

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
        *   insert n-gram with frequency
        */
        long insert(const std::string& ngram, const std::string& freq) {
            std::vector<std::string> parts = phrasit::utils::filter(
                phrasit::utils::split(phrasit::utils::trim(ngram), ' '), phrasit::notempty_filter);

            std::string cleaned_ngram = phrasit::utils::join(parts, " ");

            unsigned long n = parts.size();
            phrasit::utils::check(n <= phrasit::max_ngram,
                "n-gram size > " + std::to_string(phrasit::max_ngram));

            long id = get_id_by_ngram(cleaned_ngram);
            if (id == -1) {
                id = _max_id++;

                std::string id_str = std::to_string(id);

                storage::kvs::put(_ngram_to_id, cleaned_ngram, id_str);
                storage::kvs::put(_id_to_ngram, id_str, cleaned_ngram);
                storage::kvs::put(_freq, id_str, freq);

                // update global n-gram statistic
                long xgram_count = storage::kvs::get_ulong_or_default(_global_statistic,
                    std::to_string(n), 0);
                xgram_count++;

                storage::kvs::put(_global_statistic, std::to_string(n), std::to_string(xgram_count));

                int pos = 1;
                for (auto& x : parts) {
                    _index->append(x, id, n, pos);
                    pos++;
                }
            }
            return id;
        }

        void optimize(bool ignore_existing=false) {
            LOGINFO("optimize");
            _index->optimize(ignore_existing);

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
        *   handle a query with question mark as the only operator
        *    and return all results as a vector of n-gram ids
        */
        const std::vector<unsigned long> qmark_search(const std::string& query) {
            std::vector<unsigned long> res;
            if (query == "") {
                return res;
            }

            std::vector<std::string> parts = phrasit::utils::filter(
                phrasit::utils::split(phrasit::utils::trim(query), ' '), phrasit::notempty_filter);

            std::string cleaned_query = phrasit::utils::join(parts, " ");

            unsigned long start_pos = 0;
            while (start_pos < parts.size() && parts[start_pos] == "?") {
                start_pos ++;
            }

            // query contains only ???, and will not be supported, because of iteration over
            //  complete key set of inverted index
            if (start_pos >= parts.size()) {
                return res;
            }

            std::vector<unsigned long> result_ids = _index->get_by_key(parts[start_pos],
                parts.size(), start_pos + 1);

            // get all results via intersection of all result sets for each part
            for (unsigned long pos = start_pos + 1; pos < parts.size(); pos++) {
                auto& x = parts[pos];
                if (x == "?") {
                    continue;
                }

                auto res_for_part = _index->get_by_key(x, parts.size(), pos + 1);
                result_ids = phrasit::utils::_instersection<unsigned long>(result_ids, res_for_part);
            }

            // sort results based on n-gram frequency
            return sort_ngram_ids_by_freq(result_ids);
        }

        /*
        *   handle a query and return all results as a vector
        */
        const std::vector<unsigned long> search(const std::string& query) {
            if (query == "") {
                return {};
            }

            LOGINFO("handle query: " << query);
            std::vector<unsigned long> res;

            for (auto& q : _parser.parse(query)) {
                auto q_res = qmark_search(q);
                res = phrasit::utils::_union<unsigned long>(res, q_res);
            }

            // TODO(stg7) better merge sorted results, instead of new sorting!

            return sort_ngram_ids_by_freq(res);
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
