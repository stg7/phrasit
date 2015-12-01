/**
    part of phraseit

    \author stg7

    \brief main file

    \date 13.11.2015

    Copyright 2015 Steve Göring
**/
#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "boost/filesystem.hpp"

#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "utils/timer.hpp"
#include "phrasit.hpp"
#include "srv/webserver.hpp"
#include "consts.hpp"

#include "utils/mmfiles.hpp"
#include "utils/progress_bar.hpp"

void mmfiles_test() {
    LOGINFO("start test");
    phrasit::utils::MMArray<unsigned long> f("test_file", phrasit::utils::size::mb(100));

    f.set(100, 42);

    LOGINFO("value @ 100: " << f.get(100));

    LOGINFO("value @ 200: " << f[200]);

    for(unsigned long i = 0; i < f.size(); i++) {
        f[i] = i;
    }
}

#include <unistd.h>
void progress_bar() {
    phrasit::utils::Progress_bar pb(100);

    for (long i = 0; i < 100000; i++) {
        pb.update();
        usleep( 1000 );
    }
    std::cout << std::endl;
}

#include <deque>

void deq() {
    std::deque<int> mydeque;

    for(int i = 0; i < 10; i++) {
        mydeque.push_front(i);
    }

    while(mydeque.size() > 0) {
        int back1 = mydeque.back();
        mydeque.pop_back();

        int back2 = mydeque.back();
        mydeque.pop_back();

        std::cout << back1 << " : " << back2 << std::endl;
        mydeque.push_front(back1 + back2);

        for(auto& x: mydeque) {
            std::cout << x << ",";
        }
        std::cout << std::endl;
    }
}

#include "sort/external_sort.hpp"

void external_sort(std::string filename) {
    std::cout << "start external sorting" << std::endl;
    std::string tmppath = "./tmp";
    std::string res = phrasit::sort::external_sort(filename, tmppath);
    std::cout << "sorted values are stored in: " << res << std::endl;
}


#include <boost/network/protocol/http/server.hpp>
#include <iostream>

#include "utils/consthash.hpp"

class Webserver {
 private:
    std::string _bindaddress = "0.0.0.0";
    std::string _port = "8099";

 public:
    Webserver() {

    }

    void start() {
        try {

            boost::network::http::server<Webserver>::options options(*this);
            boost::network::http::server<Webserver> server_(options.address(_bindaddress).port(_port));
            server_.run();
        }
        catch (std::exception& e) {
            LOGERROR(e.what());
        }
    }

    void operator()(boost::network::http::server<Webserver>::request const &request,
                boost::network::http::server<Webserver>::response &response) {

        using namespace phrasit::utils;

        std::ostringstream data;

        std::string dest = request.destination;

        LOGINFO("dest:" << dest);

        switch (r_hash(dest)) {
            case c_hash("/api/"): {
                    data << "call api";
                }
                break;
            case c_hash("/help"): {
                    data << "call help";
                }
                break;
            case c_hash("/info"): {
                    data << "call info";
                }
                break;
            default: {
                    LOGERROR("processing error, unknwon path");
                }
        }

        response = boost::network::http::server<Webserver>::response::stock_reply(
            boost::network::http::server<Webserver>::response::ok, data.str());
    }
    void log(...) { }
};


void http_server_cppnetlib() {
    Webserver w;
    w.start();

}

#include "compress/gzip.hpp"

void gzip_comp() {
    std::string str = "hello";
    auto x = phrasit::compress::gzip::compress(str);
    LOGINFO("compress: " << str.length());
    LOGINFO(x.length());

    auto y = phrasit::compress::gzip::decompress(x);

    LOGINFO(y);

}

/**
    phrasit: test
**/
int main(int argc, const char* argv[]) {

    //external_sort("./storage/_ii/_tmp");
    //rocksdb_test();
    // http_server_cppnetlib();
    std::cout << "start" << std::endl;
    gzip_comp();

    std::cout << "done" << std::endl;
    return 0;
}
