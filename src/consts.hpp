/**
    part of phrasit

    \author stg7

    \brief consts module

    \date 15.11.2015

    Copyright 2016 Steve Göring

    This file is part of PhrasIt.

    PhrasIt is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PhrasIt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PhrasIt.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef CONSTS_HEADER_HPP_
#define CONSTS_HEADER_HPP_

#include <string>

namespace phrasit {
    static constexpr const char* exit_str = "!";
    static constexpr const char* shell_str = "\033[1;91mphrasit>>\033[0m ";
    static constexpr const int max_ngram = 5;
    static constexpr const int webserver_port = 8090;
    static constexpr const char* webserver_bindaddress = "0.0.0.0";
    static constexpr const bool debug = true;
    static constexpr const auto notempty_filter = [](const std::string& x){return x != "";};

    /*
    *   global variable, because it can be modified via command line arguments
    */
    static unsigned long max_result_size = 10000;

    #include "build.hpp"
}


#endif
