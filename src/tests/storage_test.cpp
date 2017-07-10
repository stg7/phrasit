/**
    part of phrasit

    \author stg7

    \brief unittest module for storage components

    \date 31.01.2016

    Copyright 2017 Steve Göring
**/
#include <string>
#include <experimental/filesystem>

#include <boost/test/unit_test.hpp>

#include "utils/log.hpp"
#include "storage/kvs.hpp"
#include "storage/inverted_index.hpp"

BOOST_AUTO_TEST_SUITE(Positronic_Storage_Test)

BOOST_AUTO_TEST_CASE(KVS_Test) {
    LOGINFO("test key value store");

    namespace fs = std::experimental::filesystem;
    std::string testdb = "tmp/_test_kvs";
    if (!fs::exists("tmp")) {
        fs::create_directory("tmp");
    }
    if (fs::exists(testdb)) {
        fs::remove_all(testdb);
    }

    phrasit::storage::kvs::type store;
    phrasit::storage::kvs::open(testdb, &store);

    LOGINFO("insert data");
    for (long i = 0; i < 100; i++) {
        phrasit::storage::kvs::put(store, std::to_string(i), std::to_string(i));
    }

    LOGINFO("check inserted data");
    for (unsigned long i = 0; i < 100; i++) {
        std::string value = "";
        bool res = phrasit::storage::kvs::get(store, std::to_string(i), &value);
        BOOST_CHECK(res == true && value == std::to_string(i));
    }

    LOGINFO("check inserted data via get_ulong");
    for (unsigned long i = 0; i < 100; i++) {
        unsigned long res = phrasit::storage::kvs::get_ulong_or_default(store, std::to_string(i), 400);
        BOOST_CHECK(res == i);
    }

    {
        LOGINFO("check not inserted keys");
        unsigned long res = phrasit::storage::kvs::get_ulong_or_default(store, "102", 400);
        BOOST_CHECK(res == 400);
    }
    phrasit::storage::kvs::close(store);
    LOGINFO("reopen store and check if values are accessible");

    {
        phrasit::storage::kvs::open(testdb, &store);
        unsigned long res = phrasit::storage::kvs::get_ulong_or_default(store, "10", 400);
        BOOST_CHECK(res == 10);
        phrasit::storage::kvs::close(store);

    }
    LOGINFO("KVS module ok.");
}

BOOST_AUTO_TEST_CASE(Inverted_Index_Test) {
    LOGINFO("test inverted index");

    namespace fs = std::experimental::filesystem;
    std::string testdb = "tmp/_test_ii";
    if (!fs::exists("tmp")) {
        fs::create_directory("tmp");
    }
    if (fs::exists(testdb)) {
        fs::remove_all(testdb);
    }

    fs::create_directory(testdb);

    LOGINFO("store some elements");
    {
        phrasit::storage::Inverted_index index(testdb);

        for (unsigned long i = 0; i < 100; i++) {
            for (unsigned long j = 0; j < 3; j++) {
                std::string token = std::to_string(i);
                index.append(token, i + j, 2, 0);
            }
        }

        index.optimize();
    }

    LOGINFO("access elements");
    {
        phrasit::storage::Inverted_index index(testdb);
        for (unsigned long i = 0; i < 100; i++) {
            BOOST_CHECK(index.get_by_key(std::to_string(i)).size() != 0);
        }
    }

    LOGINFO("inverted index module ok.");
}

BOOST_AUTO_TEST_SUITE_END()
