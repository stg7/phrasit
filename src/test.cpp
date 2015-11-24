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



void http_server() {
}

#include "sort/external_sort.hpp"

void external_sort(std::string filename) {
    std::cout << "start external sorting" << std::endl;
    std::string tmppath = "./tmp";
    std::string res = phrasit::sort::external_sort(filename, tmppath);
    std::cout << "sorted values are stored in: " << res << std::endl;
}

/**
    phrasit: test
**/
int main(int argc, const char* argv[]) {

    external_sort("./storage/_ii/_tmp");


    return 0;
}
