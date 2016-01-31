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
#include "parser/query_parser.hpp"

BOOST_AUTO_TEST_SUITE(Positronic_Parser_Test)

BOOST_AUTO_TEST_CASE(Parser_Test) {
    LOGINFO("test query parser");

    phrasit::parser::Query_parser qp;

    auto query_check = [&qp](std::string query, unsigned long required_size=0) {
        auto res = qp.parse(query);
        BOOST_CHECK(res.size() == required_size);
    };

    /*
    hello
    hello ?
    hello ? ?
    hello ? ? ?
    hello ? ? ? ?
    */
    query_check("hello *", 5);

    /*
    hello
    hello ?
    hello ? ?
    hello ? ? ?
    hello ? ? ? ?
    ? hello
    ? hello ?
    ? hello ? ?
    ? hello ? ? ?
    ? ? hello
    ? ? hello ?
    ? ? hello ? ?
    ? ? ? hello
    ? ? ? hello ?
    ? ? ? ? hello
    */
    query_check("* hello *", 15);

    /*
    hello a
    hello ? a
    hello ? ? a
    hello ? ? ? a
    ? hello a
    ? hello ? a
    ? hello ? ? a
    ? ? hello a
    ? ? hello ? a
    ? ? ? hello a
    */
    query_check("* hello * a", 10);

    /*
    hello
    ? hello
    ? ? hello
    ? ? ? hello
    ? ? ? ? hello
    */
    query_check("* hello", 5);

    /*
    hello a house
    hello house a
    */
    query_check("hello {a house} ", 2);

    /*
    hello
    hello a
    hello house
    hello ?
    hello a ?
    hello house ?
    hello ? ?
    hello a ? ?
    hello house ? ?
    */
    query_check("hello [a house] *", 9);

    /*
    o
    a o
    w o
    house o
    */
    query_check(" [a w house] o", 4);

    /*
    of
    of la
    of lo
    a of
    a of la
    a of lo
    w of
    w of la
    w of lo
    house of
    house of la
    house of lo
    */
    query_check(" [a w house] of [la lo]", 12);

    /*
    hello a
    hello a ?
    hello a ? ?
    hello a ? ? ?
    hello ? a
    hello ? a ?
    hello ? a ? ?
    hello ? ? a
    hello ? ? a ?
    hello ? ? ? a
    ? hello a
    ? hello a ?
    ? hello a ? ?
    ? hello ? a
    ? hello ? a ?
    ? hello ? ? a
    ? ? hello a
    ? ? hello a ?
    ? ? hello ? a
    ? ? ? hello a
    */
    query_check("* hello * a *", 20);

    /*
    hello a b
    hello a ? b
    hello a ? ? b
    hello ? a b
    hello ? a ? b
    hello ? ? a b
    ? hello a b
    ? hello a ? b
    ? hello ? a b
    ? ? hello a b
    */
    query_check("* hello * a * b", 10);

    /*
    hello a b
    hello a b ?
    hello a b ? ?
    hello a ? b
    hello a ? b ?
    hello a ? ? b
    hello ? a b
    hello ? a b ?
    hello ? a ? b
    hello ? ? a b
    ? hello a b
    ? hello a b ?
    ? hello a ? b
    ? hello ? a b
    ? ? hello a b
    */
    query_check("* hello * a * b *", 15);

    /*
    hello a b
    hello a b ?
    hello a b ? ?
    hello a ? b
    hello a ? b ?
    hello a ? ? b
    hello ? a b
    hello ? a b ?
    hello ? a ? b
    hello ? ? a b
    */
    query_check("hello * a * b *", 10);

    LOGINFO("query parser module ok.");
}


BOOST_AUTO_TEST_SUITE_END()
