/**
    part of phrasit

    \author stg7

    \brief logging module

    \date 17.08.2014

    Copyright 2015 Steve Göring
**/

#ifndef HELPER_HEADER_HPP_
#define HELPER_HEADER_HPP_

#include <string>
#include <vector>

namespace phrasit {
    namespace utils {
        inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
            std::stringstream ss(s);
            std::string item;
            while (std::getline(ss, item, delim)) {
                elems.push_back(item);
            }
            return elems;
        }

        inline std::vector<std::string> split(const std::string &s, char delim) {
            std::vector<std::string> elems;
            split(s, delim, elems);
            return elems;
        }
    }
}

#endif