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

/**
    phrasit: test
**/
int main(int argc, const char* argv[]) {
    LOGINFO("start test");
    phrasit::utils::MMArray<unsigned long> f("test_file", phrasit::utils::size::mb(100));

    f.set(100, 42);

    LOGINFO("value @ 100: " << f.get(100));

    LOGINFO("value @ 200: " << f[200]);

    for(unsigned long i = 0; i < f.size(); i++) {
        f[i] = i;
    }

    return 0;
}
