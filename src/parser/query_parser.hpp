/**
    part of phrasit

    \author stg7

    \brief query parser

    \date 03.12.2015

    Copyright 2017 Steve Göring

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

#ifndef QUERY_PARSER_HEADER_HPP_
#define QUERY_PARSER_HEADER_HPP_

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>
#include <set>

#include "consts.hpp"
#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "utils/timer.hpp"

namespace phrasit {
    namespace parser {
        class Query_parser {
         private:
            /*
            *   clean a query, remove double spaces
            */
            static std::string clean_query(const std::string query) {
                using namespace phrasit::utils;
                std::vector<std::string> parts = filter(split(trim(query), ' '),
                    phrasit::notempty_filter);
                return join(parts, " ");
            }

            // generic operator expansion
            /*
            *   apply generic operator on a query
            *    an operator need to have a left_bracket and a right_bracket, otherwise
            *    nothing will happen
            *    for each bracket pair an 'expansion' function will be performed, because
            *    all operators can be transformed to queries using questionmarks via
            *    a simple expansion.
            */
            static std::vector<std::string> operator_generic(std::string query,
                    const std::string left_bracket, const std::string right_bracket,
                    std::function<void(std::queue<std::string>& queries_queue,
                        const std::string left,
                        const std::string middle,
                        const std::string right)> expansion) {
                // check if there are brackets in the query string
                if (query.find(left_bracket) == std::string::npos ||
                        query.find(right_bracket) == std::string::npos) {
                    return {query};
                }

                using namespace phrasit::utils;

                std::vector<std::string> parts = filter(split(trim(query), ' '),
                    phrasit::notempty_filter);

                unsigned long count_of_open_brackets = std::count_if(parts.begin(), parts.end(),
                    [&left_bracket](const std::string& s) {return s == left_bracket;});
                unsigned long count_of_closed_brackets = std::count_if(parts.begin(), parts.end(),
                    [&right_bracket](const std::string& s) {return s == right_bracket;});

                // check if query is valid
                if (count_of_closed_brackets != count_of_open_brackets) {
                    return {};
                }

                std::vector<std::string> result_queries = {};

                // get left, inner and right part of operator and build new queries via expansion
                std::queue<std::string> queries;
                queries.push(clean_query(query));

                while (!queries.empty()) {
                    std::string current_query = queries.front();
                    queries.pop();

                    unsigned long left_pos = current_query.find(left_bracket);
                    unsigned long right_pos = current_query.find(right_bracket);

                    if (left_pos == std::string::npos || right_pos == std::string::npos) {
                        result_queries.emplace_back(current_query);
                    } else {
                        auto left = current_query.substr(0, left_pos);
                        auto middle = current_query.substr(left_pos + 1, right_pos - left_pos - 1);
                        auto right = current_query.substr(right_pos + 1);

                        expansion(queries, left, middle, right);
                    }
                }

                return result_queries;
            }

            // asterisk operator *
            /*
            Suppose operator ? is implemented, then * is no problem:
            ```
                query = "a" * "b"

                generate new candidate queries with ? up to 5 words:
                    "a" "b"
                    "a" ? "b"
                    "a" ? ? "b"
                    "a" ? ? ? "b"
                get results for each query and sort by frequency
            ```
            */
            static std::vector<std::string> operator_asterisk(std::string query) {
                LOGINFO(__FUNCTION__);
                static auto asterisk_expansion = [](std::queue<std::string>& q,
                            const std::string left,
                            const std::string middle,
                            const std::string right) {
                        using namespace phrasit::utils;

                        static constexpr const auto notempty_and_ignore_asterisk_filter =
                             [](const std::string& x){return x != "" && x != "*";};

                        static const std::vector<std::string> expansion_sequence = {
                            "",
                            "?",
                            "? ?",
                            "? ? ?",
                            "? ? ? ?"
                            };

                        for (auto& x : expansion_sequence) {
                            auto new_query = left + " " + x + " " + right;

                            std::vector<std::string> parts = filter(split(trim(new_query), ' '),
                                notempty_and_ignore_asterisk_filter);

                            if (parts.size() <= phrasit::max_ngram) {
                                q.push(clean_query(new_query));
                            }
                        }
                    };
                return operator_generic(query, "*", "*", asterisk_expansion);
            }

            // optionset operator [ ]
            /*
            [ ] can be implemented in the following way:
            ```
                query = "a" ["x" "y" "z" .. ] "b"

                build queries in a rond robin way start with empty word as filler:
                    "a" "b"
                    "a" "x" "b"
                    "a" "y" "b"
                    "a" "z" "b"
                    ....

                get each result and sort by frequency
            ```
            */
            static std::vector<std::string> operator_optionset(std::string query) {
                LOGINFO(__FUNCTION__);
                static auto optionset_expansion = [](std::queue<std::string>& q,
                            const std::string left,
                            const std::string middle,
                            const std::string right) {
                        using namespace phrasit::utils;
                        q.push(clean_query(left + " " + right));
                        for (auto& x : filter(split(trim(middle), ' '), phrasit::notempty_filter)) {
                            auto new_query = left + " " + x + " " + right;
                            q.push(clean_query(new_query));
                        }
                    };
                return operator_generic(query, "[", "]", optionset_expansion);
            }

            // orderset { }
            /*
            { } implementation:
            ```
                query = "a" \{ "x" "y" "z" \} "b"
                build each permutation of "x" "y" "z" .. (maximum 5 words)
                    "a" "x" "y" "z" "b"
                    "a" "y" "x" "z" "b"
                    "a" "y" "z" "x" "b"
                    "a" "z" "y" "x" "b"
                    "a" "z" "x" "y" "b"
                    "a" "x" "z" "y" "b"
            ```
            */
            static std::vector<std::string> operator_orderset(std::string query) {
                LOGINFO(__FUNCTION__);

                static auto orderset_expansion = [](std::queue<std::string>& q,
                            const std::string left,
                            const std::string middle, const std::string right) {
                        using namespace phrasit::utils;

                        auto middle_parts = filter(split(trim(middle), ' '), phrasit::notempty_filter);
                        if (middle_parts.size() > phrasit::max_ngram) {
                            return;
                        }

                        do {
                            std::string new_query = left + " " + join(middle_parts, " ") + " " + right;
                            q.push(clean_query(new_query));
                        } while (std::next_permutation(std::begin(middle_parts), std::end(middle_parts)));
                    };
                return operator_generic(query, "{", "}", orderset_expansion);
            }

         public:
            Query_parser() {
                LOGINFO("create query parser");
            }

            ~Query_parser() {
                LOGINFO("destroy query parser");
            }

            const std::vector<std::string> parse(const std::string& query) const {
                LOGINFO("parse query: " << query);
                phrasit::utils::Timer t;

                if (query == "") {
                    return {};
                }

                using namespace phrasit::utils;

                std::vector<std::string> parts = filter(split(trim(query), ' '),
                    phrasit::notempty_filter);

                std::string cleaned_query = join(parts, " ");

                std::vector<std::string> queries = {};

                auto operators = {&operator_asterisk, &operator_optionset, &operator_orderset};

                // do parsing of query, apply each operator and collect all queries in a list
                std::queue<std::string> generated_queries;
                generated_queries.push(cleaned_query);
                std::queue<std::string> generated_queries_tmp;

                for (auto& op : operators) {
                    while (!generated_queries.empty()) {
                        auto q = generated_queries.front();
                        generated_queries.pop();
                        for (auto& x : op(q)) {
                            generated_queries_tmp.push(x);
                        }
                    }

                    while (!generated_queries_tmp.empty()) {
                        generated_queries.push(generated_queries_tmp.front());
                        generated_queries_tmp.pop();
                    }
                }

                while (!generated_queries.empty()) {
                    auto x = generated_queries.front();
                    generated_queries.pop();
                    queries.emplace_back(x);
                }

                LOGDEBUG("needed time: " << t.time() << " ms");
                LOGDEBUG("generated queries:");
                for (auto& q: queries) {
                    LOGDEBUG(q);
                }
                return queries;
            }
        };
    }
}
#endif
