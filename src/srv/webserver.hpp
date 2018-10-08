/**
    part of phrasit

    \author stg7

    \brief webserver module

    \date 16.11.2015

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

#ifndef SERVER_HEADER_HPP_
#define SERVER_HEADER_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <map>

#include <httplib.h>

#include "utils/consthash.hpp"
#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "utils/timer.hpp"
#include "consts.hpp"
#include "phrasit.hpp"

namespace phrasit {

    namespace srv {


        class Webserver {
         private:
            Phrasit& _phrasit;
            std::string _query_log_filename;

            // based on: http://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
            /*
            *   escape a string as json
            *   FIXME: check rapidjson
            */
            std::string escapeJsonString(const std::string& input) {
                std::ostringstream ss;
                for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
                    switch (*iter) {
                        case '\\': ss << "\\\\"; break;
                        case '"': ss << "\\\""; break;
                        case '/': ss << "\\/"; break;
                        case '\b': ss << "\\b"; break;
                        case '\f': ss << "\\f"; break;
                        case '\n': ss << "\\n"; break;
                        case '\r': ss << "\\r"; break;
                        case '\t': ss << "\\t"; break;
                        default: ss << *iter; break;
                    }
                }
                return ss.str();
            }

            /*
            *   write a submitted query to the query log file, store ip adress, time and date
            */
            void log_query(const std::string& query, const std::string& ipa="0.0.0.0") {
                std::ofstream logfile;
                logfile.open(_query_log_filename, std::ofstream::out | std::ofstream::app);

                auto now = std::chrono::system_clock::now();
                auto now_c = std::chrono::system_clock::to_time_t(now);
                auto timestamp = std::put_time(std::localtime(&now_c), "%c");

                logfile << "{\"timestamp\": \"" << timestamp << "\","
                    << " \"query\": " << "\"" << escapeJsonString(query)
                    << "\", \"ipa\": \"" << ipa << "\"}\n";

                logfile.close();
            }

         public:
            Webserver(Phrasit& phrasit, const std::string& storagedir): _phrasit(phrasit) {
                _query_log_filename = storagedir + "/_query_log";
                LOGINFO("create webserver: query log file: " << _query_log_filename);
            }

            ~Webserver() {
                LOGINFO("delete webserver");
            }

            void start() {
                LOGINFO("start webserver");
                try {
                    using namespace httplib;

                    Server svr;

                    svr.Get("/api", [&](const Request& req, Response& res) {
                        auto ipa = req.headers.find("REMOTE_ADDR")->second;
                        std::map<std::string, std::string> pmap;
                        for (auto it = req.params.begin(); it != req.params.end(); ++it) {
                           const auto& x = *it;
                           pmap[x.first] = x.second;
                        }

                        std::ostringstream data;
                        query(pmap, data, ipa);
                        res.set_content(data.str(), "text/json");
                    });

                    svr.Get("/stats", [&](const Request& req, Response& res) {
                        std::ostringstream data;
                        data << "<pre>";
                        _phrasit.print_stats(data);
                        data << "</pre>";
                        res.set_content(data.str(), "text/html");
                    });

                    svr.Get("/help", [&](const Request& req, Response& res) {
                        std::ostringstream data;
                        data << "<pre>";
                        data << "you can call: \n";
                        data << "   ./help : to get this screen \n";
                        data << "   ./stats : to get statistics about phrasit \n";
                        data << "   ./api?query=whatever : to submit a query 'whatever' to phrasit \n";
                        data << "</pre>";
                        res.set_content(data.str(), "text/html");
                    });

                    LOGINFO("example: http://localhost:" << phrasit::webserver_port << "/api?query=test");
                    svr.listen("0.0.0.0", phrasit::webserver_port);
                }
                catch (std::exception& e) {
                    LOGERROR(e.what());
                }
            }

            /*
            *   handle a query and build result in json
            *
            *   \param params query parameters, e.g. params["query"] = "hello*"
            *   \param response stream for result response
            */
            void query(std::map<std::string, std::string>& params,
                    std::ostringstream& response,
                    std::string ipa = "") {
                std::string query = params["query"];
                LOGINFO("query: " << query);

                unsigned long limit = 100;
                try {
                    limit = std::stol(params["limit"]);
                } catch (...) {
                    limit = 100;
                }

                if (query == "") {
                    if (params["callback"] != "") {
                        response << params["callback"] << " ({\n"
                            << "\"error\" : \"not a valid query\""
                            << "});" << std::endl;
                    } else {
                        response << "{ \"error\" : \"not a valid query\" }" << std::endl;
                    }
                    return;
                }

                log_query(query, ipa);
                phrasit::utils::Timer t;

                std::ostringstream res;
                res << "[";
                auto& results = _phrasit.search(query, limit);
                if (results.size() > 0) {
                    unsigned long i;
                    for (i = 0; i < results.size() - 1; i++) {
                        auto& r = results[i];
                        res << "{ \""
                            << escapeJsonString(_phrasit.get_ngram(r)) << "\": "
                            << _phrasit.get_freq(r)
                            << "},";
                    }

                    auto& r = results[i];
                    res << "{ \""
                        << escapeJsonString(_phrasit.get_ngram(r)) << "\": "
                        << _phrasit.get_freq(r)
                        << "}]";
                } else {
                    res << "]";
                }

                double needed_time = t.time();
                if (params["callback"] != "") {
                    response << params["callback"] << " ({\n"
                        << "\"query\" : \"" << query << "\",\n"
                        << "\"result\" : " << res.str() << ",\n"
                        << "\"time\" : " << needed_time << "\n"
                        << "});" << std::endl;
                } else {
                    response << "{\n"
                        << "\"query\" : \"" << query << "\",\n"
                        << "\"result\" : " << res.str() << ",\n"
                        << "\"time\" : " << needed_time << "\n"
                        << "}" << std::endl;
                }
            }
        };
    }
}
#endif
