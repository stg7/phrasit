/**
    part of phrasit

    \author stg7

    \brief unittest module for utils components

    \date 31.01.2016

    Copyright 2016 Steve Göring
**/
#include <string>
#include <vector>
#include <algorithm>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "utils/consthash.hpp"
#include "utils/helper.hpp"
#include "utils/mmfiles.hpp"

BOOST_AUTO_TEST_SUITE(Positronic_Utils_Test)

BOOST_AUTO_TEST_CASE(Consthash_Test) {
    LOGINFO("test consthash");

    BOOST_CHECK(phrasit::utils::r_hash("hello") == phrasit::utils::c_hash("hello"));
    BOOST_CHECK(phrasit::utils::r_hash("helloa") != phrasit::utils::c_hash("hello"));
    BOOST_CHECK(phrasit::utils::r_hash("  helloaaaaaaaaaaa   ") ==
        phrasit::utils::c_hash("  helloaaaaaaaaaaa   "));
    BOOST_CHECK(phrasit::utils::r_hash("") == phrasit::utils::c_hash(""));

    LOGINFO("consthash module ok.");
}

BOOST_AUTO_TEST_CASE(Helper_Test) {
    LOGINFO("test helper");

    {
        LOGINFO(" intersection");
        std::vector<int> v1 = {9, 3, 4, 8, 2, 3};
        std::vector<int> v2 = {1, 2, 3, 8, 0};
        std::vector<int> v3 = {10, 11};
        auto inter = phrasit::utils::_intersection<int>(v1, v2);

        BOOST_CHECK(inter.size() == 3);
        for (auto x: {2, 3, 8}) {
            BOOST_CHECK(std::find(inter.begin(), inter.end(), x) != inter.end());
        }

        auto empty = phrasit::utils::_intersection<int>(v1, v3);
        BOOST_CHECK(empty.size() == 0);
    }
    {
        LOGINFO(" union");
        std::vector<int> v1 = {9, 3, 4, 8, 2, 3};
        std::vector<int> v2 = {1, 2, 3, 8, 0};
        std::vector<int> v3 = {10, 11};
        auto uni = phrasit::utils::_union<int>(v1, v2);
        BOOST_CHECK(uni.size() == 7);
        for (auto x: {0, 1, 2, 3, 4, 8, 9}) {
            BOOST_CHECK(std::find(uni.begin(), uni.end(), x) != uni.end());
        }

        auto uni2 = phrasit::utils::_union<int>(v1, v3);

        BOOST_CHECK(uni2.size() == 7);
        for (auto x: {2, 3, 4, 8, 9, 10, 11}) {
            BOOST_CHECK(std::find(uni2.begin(), uni2.end(), x) != uni2.end());
        }
    }
    {
        LOGINFO(" split");
        auto res = phrasit::utils::split("hello world !", ' ');
        BOOST_CHECK(res.size() == 3);
        for (auto x: {"hello", "world", "!"}) {
            BOOST_CHECK(std::find(res.begin(), res.end(), x) != res.end());
        }
        auto res_one = phrasit::utils::split("hello world !", 'X');
        BOOST_CHECK(res_one.size() == 1);
    }
    {
        LOGINFO(" trim");
        BOOST_CHECK(phrasit::utils::trim("    hello world   ") == "hello world");
        BOOST_CHECK(phrasit::utils::trim("hello world   ") == "hello world");
        BOOST_CHECK(phrasit::utils::trim("    hello world") == "hello world");
        BOOST_CHECK(phrasit::utils::trim("\thello world") == "\thello world");
        BOOST_CHECK(phrasit::utils::trim("hello world\n") == "hello world\n");
    }
    {
        LOGINFO(" join");
        BOOST_CHECK(phrasit::utils::join({"1", "2", "3"}, "!") == "1!2!3");
        BOOST_CHECK(phrasit::utils::join({"1", "2"}, "xx") == "1xx2");
    }
    {
        LOGINFO(" filter");
        auto fi = [](const std::string& x) {return x != "3";};
        auto res = phrasit::utils::filter({"1", "2", "3"}, fi);
        BOOST_CHECK(res.size() == 2);
        BOOST_CHECK(res[0] == "1");
        BOOST_CHECK(res[1] == "2");
    }

    LOGINFO("helper module ok.");
}

BOOST_AUTO_TEST_CASE(Mmfiles_Test) {
    LOGINFO("test mmfiles");

    namespace fs = boost::filesystem;
    std::string testfile = "tmp/_test_mmf";
    if (!fs::exists("tmp")) {
        fs::create_directory("tmp");
    }
    if (fs::exists(testfile)) {
        fs::remove(testfile);
    }

    {
        LOGINFO("write test");
        phrasit::utils::MMArray<unsigned long> f(testfile, phrasit::utils::size::mb(100));
        BOOST_CHECK(f.is_open());

        f.set(100, 42);

        BOOST_CHECK(f.get(100) == 42);
        BOOST_CHECK(f[100] == 42);

        f[200] = 123;
        BOOST_CHECK(f[200] == 123);

        for(unsigned long i = 0; i < f.size(); i++) {
            f[i] = i;
        }

        for(unsigned long i = 0; i < f.size(); i++) {
            BOOST_CHECK(f[i] == i);
        }
        f.close();
    }

    {
        LOGINFO("read test");
        phrasit::utils::MMArray<unsigned long> f(testfile);

        BOOST_CHECK(f.is_open());

        for(unsigned long i = 0; i < f.size(); i++) {
            BOOST_CHECK(f[i] == i);
        }
        f.close();
    }
    fs::remove(testfile);

    LOGINFO("mmfiles module ok.");
}

BOOST_AUTO_TEST_SUITE_END()
