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

#include <mongoose/Server.h>
#include <mongoose/WebController.h>

#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "utils/timer.hpp"
#include "consts.hpp"
#include "phrasit.hpp"

namespace phrasit {

    namespace srv {

        using namespace Mongoose;

        class Webserver : public WebController {
         private:
            Phrasit& _phrasit;
         public:
            Webserver(Phrasit& phrasit): _phrasit(phrasit) {
                LOGINFO("start webserver");

            }

            ~Webserver() {
                LOGINFO("end webserver");
            }

            void query(Request &request, StreamResponse &response) {
                std::string query = htmlEntities(request.get("query", ""));
                std::cout << "query=" << query << std::endl;
                if (query == "") {
                    response << "{\"error\":\"not a valid query\"}" << std::endl;
                    return;
                }
                phrasit::utils::Timer t;

                std::string res = "";
                for(auto& r : _phrasit.search(query)) {
                    res += r;
                }

                double needed_time = t.time();
                response << "{"
                    << "\"query\":\"" << query << "\",\n"
                    << "\"result\":\"" << res << "\",\n"
                    << "\"time\":" << needed_time
                    << "}" << std::endl;
            }

            void setup() {
                setPrefix("/api");
                addRoute("GET", "/", Webserver, query);
            }

            void start() {
                Server server(phrasit::webserver_port);
                server.registerController(this);
                server.start();
                LOGINFO("server running on port: " << phrasit::webserver_port);
                LOGINFO("example: http://localhost:" << phrasit::webserver_port << "/api/?query=test");
                this->dumpRoutes();

                while (1) {
                    usleep(1000000);
                }
            }
        };
    }
}
#endif
