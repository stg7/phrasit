/**
    part of phrasit

    \author stg7

    \brief unittest module for sorting components

    \date 31.01.2016

    Copyright 2016 Steve Göring
**/
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "utils/log.hpp"
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

    for(auto x: strings_copy) {
        LOGINFO(x);
    }

    LOGINFO("parallel sort module ok.");
}

BOOST_AUTO_TEST_CASE(External_Sort_Test) {
    LOGINFO("test external sort");
    /*
    namespace fs = boost::filesystem;
    std::string testdb = "tmp/_test_ii";
    if (!fs::exists("tmp")) {
        fs::create_directory("tmp");
    }
    if (fs::exists(testdb)) {
        fs::remove_all(testdb);
    }

    fs::create_directory(testdb); */



    LOGINFO("external sort module ok.");
}

BOOST_AUTO_TEST_SUITE_END()
