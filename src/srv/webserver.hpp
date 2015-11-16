/**
    part of phrasit

    \author stg7

    \brief webserver module

    \date 16.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef SERVER_HEADER_HPP_
#define SERVER_HEADER_HPP_

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

            void index(Request &request, StreamResponse &response) {
                response << "{\"error\":\"not a valid request\"}" << std::endl;
            }

            void setup() {
                addRoute("GET", "/", Webserver, index);
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
