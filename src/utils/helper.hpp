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
#include <functional>

namespace phrasit {
    namespace utils {

        /**
        *   filter a given string vector `values` by a predicate `f`
        *   \param values a string vector
        *   \param f predicate to apply
        *   \return a new vector with values that matches the predicate `f`
        */
        inline std::vector<std::string> filter(std::vector<std::string> values, std::function<bool (std::string&)> f) {
            std::vector<std::string> res;
            for (auto& x : values) {
                if (f(x)) {
                    res.emplace_back(x);
                }
            }
            return res;
        }

        /*
        *   joins a vector of strings with connector between each element
        *   \param values a string vector
        *   \param connector string that is between each element
        *   \return joined string
        *
        *   example:
        *       join(["a", "b"], "!") will return "a!b"
        */
        inline std::string join(std::vector<std::string> values, const std::string& connector) {
            std::string res = "";

            for (auto it = values.begin(); it != values.end(); it++) {
                res += *it;
                if (it != values.end()) {
                    res += connector;
                }
            }
            return res;
        }

        /*
        *   trim spaces from a string
        *
        *   \param s string for trimming
        *   \return string s without leading/trailing spaces
        */
        inline std::string trim(const std::string &s) {
            std::string::const_iterator it = s.begin();
            while (it != s.end() && isspace(*it)) {
                it++;
            }

            std::string::const_reverse_iterator rit = s.rbegin();
            while (rit.base() != it && isspace(*rit)) {
                rit++;
            }

            return std::string(it, rit.base());
        }

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
