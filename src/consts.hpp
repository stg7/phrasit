/**
    part of phrasit

    \author stg7

    \brief consts module

    \date 15.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef CONSTS_HEADER_HPP_
#define CONSTS_HEADER_HPP_

namespace phrasit {
    static constexpr const char* exit_str = "!";
    static constexpr const char* shell_str = "\033[1;91mphrasit>>\033[0m ";
    static constexpr const int max_ngram = 5;
    static constexpr const int webserver_port = 8090;
    static constexpr const bool debug = true;
    static constexpr const auto notempty_filter = [](const std::string& x){return x != "";};

    #include "build.hpp"
}


#endif
