/**
    part of phrasit

    \author stg7

    \brief Phrasit module

    \date 15.11.2015

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

#ifndef PHRASIT_HEADER_HPP_
#define PHRASIT_HEADER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <limits>
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

    template<typename T> inline T quick_select(std::vector<T> list, unsigned long left, unsigned long right, unsigned long k) {
        // based on: https://en.wikipedia.org/wiki/Quickselect
        if (left == right) {
            return list[left];
        }

        unsigned long pivot = left + (right - left) / 2;
        const auto partition = [&list](unsigned long left, unsigned long right, unsigned long pivot) -> unsigned long {
            T pivotValue = list[pivot];
            list[pivot] = list[right];
            list[right] = pivotValue;

            unsigned long store_index = left;

            for (unsigned long i = left; i < right; i++) {
                if (list[i] < pivotValue) {
                    T tmp = list[store_index];
                    list[store_index] = list[i];
                    list[i] = tmp;
                    store_index ++;
                }
            }
            T tmp = list[right];
            list[right] = list[store_index];
            list[store_index] = tmp;

            return store_index;
        };

        pivot = partition(left, right, pivot);
        if (k == pivot) {
            return list[pivot];
        }
        if (k < pivot) {
            return quick_select(list, left, pivot - 1, k);
        }
        return quick_select(list, pivot + 1, right, k);
    }

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
        const inline std::vector<unsigned long> sort_ngram_ids_by_freq(const std::vector<unsigned long>& result_ids, const unsigned long limit = 100) const {
            phrasit::utils::Timer t;

            std::vector<unsigned long> res(std::min(phrasit::max_result_size, limit));

            // get frequencies of all n-grams and select top-limit ones

            std::vector<std::tuple<unsigned long, unsigned long>> freqs_vec;
            std::vector<unsigned long> freqs(result_ids.size());

            for (unsigned long i = 0; i < result_ids.size(); i++) {
                auto& x = result_ids[i];
                freqs[i] = get_freq(x);
            }
            // perform quick select, to get a lower frequency bound
            unsigned long minfreq = quick_select<unsigned long>(freqs, 0, freqs.size(), res.size());


            for (unsigned long i = 0; i < result_ids.size(); i++) {
                auto& x = result_ids[i];
                auto freq = get_freq(x);
                if (freq >= minfreq) {
                    freqs_vec.emplace_back(std::make_tuple(x ,freq));
                }
            }

            std::sort(freqs_vec.begin(), freqs_vec.end(),
                [](const auto& a, const auto& b) -> bool {
                    return  std::get<1>(a) > std::get<1>(b);
                });

            for(unsigned long i = 0; i < std::min(phrasit::max_result_size, limit); i++) {
                auto& x = freqs_vec[i];
                res[i] = std::get<0>(x);
            }
            /*

            auto cmp = [&freq_map](unsigned long& left, unsigned long& right) -> bool {
                return freq_map[left] > freq_map[right];
            };

            std::priority_queue<unsigned long, std::vector<unsigned long>, decltype(cmp) > queue(cmp);

            unsigned long min_freq = 0;
            for (auto& x : result_ids) {
                if (freq_map[x] > min_freq) {
                    queue.push(x);
                }

                // remove elements from queue to reduce memory overhead
                //  when a query will receive a lot of results
                if (queue.size() > std::min(phrasit::max_result_size, limit)) {
                    min_freq = freq_map[queue.top()];
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
            */
            LOGDEBUG("needed time for sort: " << t.time() << " ms");
            return res;
        }

        inline void update_max_key() {
            storage::kvs::put(_ngram_to_id, _max_id_key, std::to_string(_max_id));
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
            // try to restore max id count
            if (_max_id == 0) {
                LOGINFO("restore max_id");
                _max_id = storage::kvs::count_of_keys(_ngram_to_id);
            }

            LOGINFO("initialize store with max_id: " << _max_id);
        }

        ~Phrasit() {
            print_stats(std::cout);

            LOGINFO("delete store");
            close();
        }

        inline void close() {
            update_max_key();

            storage::kvs::close(_ngram_to_id);
            storage::kvs::close(_id_to_ngram);
            storage::kvs::close(_freq);
            storage::kvs::close(_global_statistic);
        }

        const long get_id_by_ngram(const std::string& ngram) const {
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
            utils::check(n <= phrasit::max_ngram,
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
                update_max_key();

                int pos = 1;
                for (auto& x : parts) {
                    _index->append(x, id, n, pos);
                    pos++;
                }
            }
            return id;
        }

        void optimize(const bool ignore_existing = false) {
            LOGINFO("optimize");
            _index->optimize(ignore_existing);
        }

        const std::string get_ngram(const unsigned long id) const {
            std::string ngram = "";
            storage::kvs::get(_id_to_ngram, std::to_string(id), &ngram);
            return ngram;
        }

        const unsigned long get_freq(const unsigned long id) const {
            return storage::kvs::get_ulong_or_default(_freq, std::to_string(id), 0);
        }

        /*
        *   handle a query with question mark as the only operator
        *    and return all results as a vector of n-gram ids
        */
        const std::vector<unsigned long> qmark_search(const std::string& query, const bool sort_results = true) const {
            if (query == "") {
                return {};
            }

            using namespace phrasit::utils;

            std::vector<std::string> parts = filter(split(trim(query), ' '),
                phrasit::notempty_filter);

            std::string cleaned_query = join(parts, " ");

            // search for first not '?' part
            unsigned long start_pos = 0;
            while (start_pos < parts.size() && parts[start_pos] == "?") {
                start_pos++;
            }

            // query contains only ???, and will not be supported, because of possible
            //  iteration over complete key set of inverted index for answering this query
            if (start_pos >= parts.size()) {
                return {};
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
                result_ids = _intersection<unsigned long>(result_ids, res_for_part, true);
            }

            if (sort_results) {
                // sort results based on n-gram frequency
                return sort_ngram_ids_by_freq(result_ids);
            }
            return result_ids;
        }

        /*
        *   handle a query and return all results as a vector
        */
        const std::vector<unsigned long> search(const std::string& query, const unsigned long limit = 100) const {
            if (query == "") {
                return {};
            }
            phrasit::utils::Timer t;
            LOGINFO("handle query: " << query);
            std::vector<unsigned long> res;

            // TODO(stg7): multiple queries can be handled parallel
            for (auto& q : _parser.parse(query)) {
                auto q_res = qmark_search(q, false);
                res = phrasit::utils::_union<unsigned long>(res, q_res, true);
            }
            LOGDEBUG("needed time: " << t.time() << " ms");
            return sort_ngram_ids_by_freq(res, limit);
        }

        /*
        *   printout global collected statistics
        */
        const void print_stats(std::ostream& out) const {
            out << "phrasit statistics:" << std::endl;
            std::string xgram_count = "";
            for (int n = 1; n < phrasit::max_ngram + 1; n++) {
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
