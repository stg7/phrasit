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
#include "consts.hpp"

namespace phrasit {
    class Phrasit;

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

                response << "{\"query\":\"" << query << "\"}" << std::endl;
                // response << "{\"error\":\"not a valid request\"}" << std::endl;
            }

            void setup() {
                addRoute("GET", "/", Webserver, query);
            }

            void start() {
                Server server(phrasit::webserver_port);
                server.registerController(this);
                server.start();

                while (1) {
                    usleep(1000000);
                }
            }
        };
    }
}
#endif
