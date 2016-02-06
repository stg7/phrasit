/**
    part of phrasit

    \author stg7

    \brief unittest module for sorting components

    \date 31.01.2016

    Copyright 2016 Steve Göring
**/
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <functional> // for std::function
#include <algorithm>  // for std::generate_n
#include <stdlib.h>  // for system call

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "compress/file.hpp"
#include "utils/log.hpp"
#include "utils/timer.hpp"
#include "sort/parallel_sort.hpp"
#include "sort/external_sort.hpp"

BOOST_AUTO_TEST_SUITE(Positronic_Sort_Test)

BOOST_AUTO_TEST_CASE(Parallel_Sort_Test) {
    LOGINFO("test parallel sort");
    std::vector<std::string> strings = {
        "b",
        "c",
        "aaaa",
        "acc",
        "ass",
        "ppppp",
        "opopop"
    };
    std::vector<std::string> strings_copy(strings);

    phrasit::sort::parallel_sort(strings);

    BOOST_CHECK(strings_copy[0] != strings[0]);

    std::sort(strings_copy.begin(), strings_copy.end());

    for(size_t i = 0; i < strings_copy.size(); i++) {
        BOOST_CHECK(strings_copy[i] == strings[i]);
    }

    LOGINFO("parallel sort module ok.");
}

std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

BOOST_AUTO_TEST_CASE(External_Sort_Test) {
    LOGINFO("test external sort");

    namespace fs = boost::filesystem;
    std::string testfile = "tmp/_test_ext_sort.gz";
    if (!fs::exists("tmp")) {
        fs::create_directory("tmp");
    }
    if (fs::exists(testfile)) {
        fs::remove(testfile);
    }

    LOGINFO("create a file for sorting");
    std::vector<std::string> values;
    {
        phrasit::compress::File<phrasit::compress::mode::write> tmp_file(testfile);

        for (unsigned long i = 0; i < 10000000; i++) {
            std::string value = random_string(rand() % 30);
            if (value != "") {
                tmp_file.writeLine(value);
                values.emplace_back(value);
            }
        }
        tmp_file.close();
    }

    std::sort(values.begin(), values.end());

    LOGINFO("sort it and perform check");
    phrasit::utils::Timer t;
    std::string tmppath = "tmp";
    std::string res = phrasit::sort::external_sort(testfile, tmppath, 30000000);
    // TODO(stg7): there is an empty line bug, if a file has an empty line somewhere
    //  external sort will remove this line, this behavior is not correct in the sense of sorting
    //  a file, but for the index creation it is ok
    {

        phrasit::compress::File<phrasit::compress::mode::read> file(res);
        unsigned long i = 0;
        for (std::string line = ""; file.readLine(line); ) {
            BOOST_CHECK(line == values[i]);
            i++;
        }
        BOOST_CHECK(i > 0);
        /*
        std::ifstream sorted_file(res);
        std::string line;
        unsigned long i = 0;
        while (std::getline(sorted_file, line)) {
            BOOST_CHECK(line == values[i]);
            i++;
        }
        */
    }
    LOGINFO("needed time: " << t.time());

    fs::remove(res);
    fs::remove(testfile);
    LOGINFO("external sort module ok.");
}

BOOST_AUTO_TEST_SUITE_END()
