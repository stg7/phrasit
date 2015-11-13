/**
    part of phrasit

    \author stg7

    \brief logging module

    \date 13.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef HELPER_HEADER_HPP_
#define HELPER_HEADER_HPP_

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace phrasit {
    namespace utils {

        inline std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems) {
            std::stringstream ss(s);
            std::string item;
            while (std::getline(ss, item, delim)) {
                elems.push_back(item);
            }
            return elems;
        }

        /*
            split a string by delimiter and return result in a vector of strings
            \param s string to split
            \param delim using this character as a delimiter
            \return splitted strings as a vector
        */
        inline std::vector<std::string> split(const std::string& s, char delim) {
            std::vector<std::string> elems;
            split(s, delim, elems);
            return elems;
        }

    }
}
#endif
