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

#include <boost/network/protocol/http/server.hpp>

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

                if (!std::getline(iss, url, '?')) { // remove the URL part
                    return "";
                }

                // store query key/value pairs in a map
                std::string keyval, key, val;

                while (std::getline(iss, keyval, '&')) { // split each term
                    std::istringstream iss(keyval);

                    if (std::getline(std::getline(iss, key, '='), val)) {
                        params[key] = val;
                    }
                }
                return url;
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

                if (query == "") {
                    response << "{\"error\":\"not a valid query\"}" << std::endl;
                    return;
                }

                phrasit::utils::Timer t;

                std::ostringstream res;
                res << "[";
                for (auto& r : _phrasit.search(query)) {
                    res << "\"" << r << "\",";
                }
                res << "\"\"" << "]";

                double needed_time = t.time();
                response << "{"
                    << "\"query\":\"" << query << "\",\n"
                    << "\"result\":" << res.str() << ",\n"
                    << "\"time\":" << needed_time
                    << "}" << std::endl;
            }

            // default operator for cppnetlib, this method will be called after a http request
            void operator()(boost::network::http::server<Webserver>::request const &request,
                        boost::network::http::server<Webserver>::response &response) {

                using namespace phrasit::utils;

                std::ostringstream data;

                std::map<std::string, std::string> params;
                std::string path = parse(request.destination, params);

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
                            data << "you can call: \n" ;
                            data << "   ./help : to get this screen \n";
                            data << "   ./stats : to get statistics about phrasit \n";
                            data << "   ./api/?query=whatever : to submit a query 'whatever' to phrasit \n";
                            data << "</pre>";
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
