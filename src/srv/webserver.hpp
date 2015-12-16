/**
    part of phrasit

    \author stg7

    \brief webserver module

    \date 16.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef SERVER_HEADER_HPP_
#define SERVER_HEADER_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include <boost/network/protocol/http/server.hpp>
#include <boost/network/uri/decode.hpp>

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

            /*
            *   parse url in params and path parts based on: http://stackoverflow.com/questions/28268236/parse-http-request-without-using-regexp
            *   \param destination full path of request with all parameters, e.g. /api/?query=42
            *   \param params map where all extracted parameters were stored
            *   \return path without query parameters
            */
            std::string parse(std::string& destination, std::map<std::string, std::string>& params) {
                std::istringstream iss(destination);
                std::string url;

                if (!std::getline(iss, url, '?')) {  // remove the URL part
                    return "";
                }

                // store query key/value pairs in a map
                std::string keyval, key, val;

                while (std::getline(iss, keyval, '&')) {  // split each term
                    std::istringstream iss(keyval);

                    if (std::getline(std::getline(iss, key, '='), val)) {
                        params[key] = val;
                    }
                }
                return url;
            }

            // based on: http://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
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

         public:
            Webserver(Phrasit& phrasit): _phrasit(phrasit) {
                LOGINFO("create webserver");
            }

            ~Webserver() {
                LOGINFO("delete webserver");
            }

            void start() {
                LOGINFO("start webserver");
                try {
                    boost::network::http::server<Webserver>::options options(*this);
                    boost::network::http::server<Webserver> server_(options.address(phrasit::webserver_bindaddress).port(std::to_string(phrasit::webserver_port)));
                    LOGINFO("example: http://localhost:" << phrasit::webserver_port << "/api/?query=test");
                    server_.run();
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
            void query(std::map<std::string, std::string>& params, std::ostringstream& response) {
                std::string query = params["query"];
                LOGINFO("query: " << query);

                unsigned long limit = 100;
                try {
                    limit = std::stol(params["limit"]);
                } catch (...) {
                    limit = 100;
                }

                if (query == "") {
                    response << "{ \"error\" : \"not a valid query\" }" << std::endl;
                    return;
                }

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

            // default operator for cppnetlib, this method will be called after a http request
            void operator()(boost::network::http::server<Webserver>::request const &request,
                        boost::network::http::server<Webserver>::response &response) {
                using namespace phrasit::utils;

                std::ostringstream data;

                std::map<std::string, std::string> params;

                std::string decoded_dest = boost::network::uri::decoded(request.destination);
                std::string path = parse(decoded_dest, params);

                LOGINFO("path:" << path);

                // based on path do the action:
                switch (r_hash(path)) {
                    case c_hash("/api/"): {
                            query(params, data);
                        }
                        break;
                    case c_hash("/help"): {
                            // TODO(stg7) this should be readed from a file
                            data << "<pre>";
                            data << "you can call: \n";
                            data << "   ./help : to get this screen \n";
                            data << "   ./stats : to get statistics about phrasit \n";
                            data << "   ./api/?query=whatever : to submit a query 'whatever' to phrasit \n";
                            data << "</pre>";
                        }
                        break;
                    case c_hash("/favicon.ico"): {
                            // TODO(stg7) return something
                        }
                        break;
                    case c_hash("/stats"): {
                            data << "<pre>";
                            _phrasit.print_stats(data);
                            data << "</pre>";
                        }
                        break;
                    default: {
                            LOGERROR("processing error, unknwon path: " << path);
                        }
                }

                response = boost::network::http::server<Webserver>::response::stock_reply(
                    boost::network::http::server<Webserver>::response::ok, data.str());
            }
            void log(...) { }
        };
    }
}
#endif
