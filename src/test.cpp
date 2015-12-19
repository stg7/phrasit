/**
    part of phraseit

    \author stg7

    \brief main file

    \date 13.11.2015

    Copyright 2015 Steve Göring
**/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>


#include "parser/query_parser.hpp"
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

#include "compress/string.hpp"

std::string bytes_str(const std::string& s) {
    std::string res = "";
    for (auto& c : s) {
        res += std::to_string((int) c) + ",";
    }
    return res;
}

#include <random>
#include <functional> //for std::function
#include <algorithm>  //for std::generate_n

std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
        "abcdefghijklmnopqrstuvwxyz ";
        //"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        //"0123456789"
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

void string_comp() {
    std::string instr = "hello and world" + random_string(10);
    auto cstr = phrasit::compress::string::compress(instr);
    LOGINFO("instr:  " << bytes_str(instr));
    LOGINFO("insize: " << instr.length());
    LOGINFO("cstr:   " << bytes_str(cstr));
    LOGINFO("csize:  " << cstr.length());

    auto y = phrasit::compress::string::decompress(cstr);
    LOGINFO("decompressed: " << y);

    phrasit::utils::check(instr == y, "decompression failed!");

    std::vector<std::string> strings;
    int max_str = 100000;
    int string_len = 140;

    long size = 0;
    for (int i = 0; i < max_str; i++) {
        strings.emplace_back(random_string(string_len));
        size += string_len;
    }

    {
        phrasit::utils::Timer t;

        std::vector<std::string> compressed_strings;
        long csize = 0;

        for (size_t i = 0; i < strings.size(); i++) {
            std::string cstr = phrasit::compress::string::compress(strings[i]);
            csize += cstr.size();
            compressed_strings.emplace_back(cstr);
        }

        auto needed_time = t.time();

        for (size_t i = 0; i < strings.size(); i++) {
            auto x = phrasit::compress::string::decompress(compressed_strings[i]);
            phrasit::utils::check(strings[i] == x, "decompression failed! \n" + strings[i]  + " \n" + x);
        }

        LOGINFO("needed time: " << needed_time);
        LOGINFO("ms/str: " << (needed_time/max_str));
        LOGINFO("size:  " << size);
        LOGINFO("csize: " << csize);
        LOGINFO("ratio: " << (((double) csize) / size));

    }
}

void intersection_test() {
    {
        std::vector<int> v1 = {9,3,4,8,2,3};
        std::vector<int> v2 = {1,2,3,8,0};

        auto inter = phrasit::utils::_intersection<int>(v1, v2);

        for (auto& x : inter) {
            std::cout << x << std::endl;
        }
    }
    // result should be {2,3,8}

    {
        std::vector<int> v1 = {9,3,4,8,2,3};
        std::vector<int> v2 = {};
        auto inter = phrasit::utils::_intersection<int>(v1, v2);

        assert(inter.size() == 0);
    }
}

void query_parser_test() {
    LOGINFO("query_parser_test");
    phrasit::parser::Query_parser qp;

    assert(qp.parse("").size() == 0);

    auto print_query_and_check = [&qp](std::string query, bool check=false, unsigned long size=0) {
        auto res = qp.parse(query);
        std::cout << "query: " << query << std::endl;
        for (auto& q : res) {
            std::cout << "  " << q << std::endl;
        }
        if (check) {
            assert(res.size() == size);
        }
    };

    print_query_and_check("hello {a house} ");
    print_query_and_check("* hello * a");

    return;

    print_query_and_check("hello [a house] *");
    print_query_and_check(" [a w house] o");
    print_query_and_check(" [a w house] of [la lo]");
    print_query_and_check("* hello *");
    print_query_and_check("hello *");
    print_query_and_check("* hello");

    print_query_and_check("* hello * a *");
    print_query_and_check("* hello * a * b");
    print_query_and_check("* hello * a * b *");
    print_query_and_check("hello * a * b *");

    return;
    assert(qp.parse("hello *").size() == 5);
    assert(qp.parse("hello").size() == 1);

    assert(qp.parse("hello ?").size() == 1);

    // hello * -> {hello , hello ?, hello ? ?, hello ? ? ? , hello ? ? ? ?}

    assert(qp.parse("* hello").size() == 5);


}

/**
    phrasit: test
**/
int main(int argc, const char* argv[]) {

    std::cout << "start" << std::endl;
    //external_sort("./storage/_ii/_tmp");
    //rocksdb_test();
    // http_server_cppnetlib();
    //string_comp();
    //intersection_test();
    query_parser_test();

    std::cout << "done" << std::endl;
    return 0;
}
