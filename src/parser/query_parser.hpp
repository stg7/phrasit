/**
    part of phrasit

    \author stg7

    \brief query parser

    \date 03.12.2015

    Copyright 2015 Steve Göring
**/

#ifndef QUERY_PARSER_HEADER_HPP_
#define QUERY_PARSER_HEADER_HPP_

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <queue>

#include "consts.hpp"
#include "utils/log.hpp"
#include "utils/helper.hpp"

namespace phrasit {
    namespace parser {
        class Query_parser {
         private:

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
                LOGINFO( __FUNCTION__);
                if (query.find("*") == std::string::npos) {
                    return {query};
                }
                using namespace phrasit::utils;
                // TODO(stg7) clean query : remove multiple **
                // query = join(filter(split(trim(query), '*'), phrasit::notempty_filter), "*");
                // ^ does not work, it removes a beginning *

                std::vector<std::string> res_queries;

                std::vector<std::string> parts = filter(split(trim(query), ' '), phrasit::notempty_filter);

                if (parts.size() > phrasit::max_ngram) {
                    return {};
                }

                static const std::vector<std::string> expansion_sequence =
                    {
                    " ",
                    " ? ",
                    " ? ? ",
                    " ? ? ? ",
                    " ? ? ? ? "
                    };

                unsigned long count_of_asterisks = std::count_if(parts.begin(), parts.end(),
                    [](const std::string& s) {return s == "*";});

                // count_of_asterisks should be lower than maximum of ngram
                if (count_of_asterisks > phrasit::max_ngram) {
                    return {};
                }

                std::queue<std::string> queries;
                queries.push(query);

                std::vector<std::string> tmp_result_queries;

                // do query expansion, for each asterisk add all possible expasions, repeat
                //  until there are no asterisks in the query
                while (!queries.empty()) {
                    auto current_query = queries.front();
                    queries.pop();

                    bool has_asterisk = false;
                    unsigned long pos = 0;
                    // TODO(stg7) there is a string find function
                    for (auto& c : current_query) {
                        if (c == '*') {
                            has_asterisk = true;
                            break;
                        }
                        pos ++;
                    }
                    if (!has_asterisk) {
                        tmp_result_queries.push_back(current_query);
                    } else {
                        for (auto& expansion : expansion_sequence) {
                            auto left = current_query.substr(0, pos);
                            auto right = current_query.substr(pos + 1, current_query.size() - pos - 1);
                            auto new_query = left + expansion + right;

                            queries.push(new_query);
                        }
                    }
                }
                // TODO(stg7) do it in a better way without filtering in the end

                // filter out queries that are too long
                for (auto& q : tmp_result_queries) {
                    std::vector<std::string> parts = filter(split(trim(q), ' '), phrasit::notempty_filter);

                    if (parts.size() <= phrasit::max_ngram) {
                        res_queries.emplace_back(join(parts, " "));
                    }
                }

                return res_queries;
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
                LOGINFO( __FUNCTION__);
                return {};
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
                LOGINFO( __FUNCTION__);
                return {};
            }

         public:
            Query_parser() {
                LOGINFO("create query parser");

            }

            ~Query_parser() {
                LOGINFO("destroy query parser");
            }

            std::vector<std::string> parse(const std::string& query) {
                LOGINFO("parse query" << query);
                if (query == "") {
                    return {};
                }

                std::vector<std::string> parts = phrasit::utils::filter(
                    phrasit::utils::split(phrasit::utils::trim(query), ' '), phrasit::notempty_filter);

                std::string cleaned_query = phrasit::utils::join(parts, " ");

                std::vector<std::string> queries;

                auto operators = {&operator_asterisk, &operator_optionset, &operator_orderset};

                // do parsing of query
                for (auto& op : operators) {
                    for (auto& x : op(cleaned_query)) {
                        queries.emplace_back(x);
                    }
                }

                return queries;
            }

        };
    }
}

#endif