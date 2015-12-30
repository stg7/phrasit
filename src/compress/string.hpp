/**
    part of phrasit

    \author stg7

    \brief simple string compression based on analyzed most frequent 2,3,4 grams

    \date 01.12.2015

    Copyright 2015 Steve Göring

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

#ifndef STRING_HEADER_HPP_
#define STRING_HEADER_HPP_

#include <string>

#include "utils/log.hpp"

namespace phrasit {
    namespace compress {
        namespace string {
            // TODO(stg7) it is not used at the moment, maybe remove it

            static std::string _comp_table[32] = {"\0", "ing", "in", "er", "re", "ti", "on", "es", "te", "an", "en", "ed", "at", "st", "al", "ng", "th", "nt", "ar", "le", "co", "or", "de", "ra", "ri", "se", "io", "li", "ic", "is", "ne", "me"};

            inline std::string compress(const std::string& data) {
                std::string res = "";

                unsigned long i = 0;
                unsigned long len = 0;
                unsigned long steps = 0;

                while (i < data.size()) {
                    bool replaced = false;

                    for (char j = 0; j < 32; j++) {
                        std::string& rep = _comp_table[(unsigned int)j];
                        len = rep.size();
                        if (rep != "\0" && data.compare(i, len, rep) == 0) {
                            res += j;
                            replaced = true;
                            break;
                        }
                    }
                    if (!replaced) {
                        len = 1;
                        res += data[i];
                    }
                    i += len;
                    steps += 1;
                }
                return res;
            }

            inline std::string decompress(const std::string& data) {
                std::string res = "";
                for (auto& c : data) {
                    unsigned int i = c;
                    if (i < 32) {
                        res += _comp_table[i];
                    } else {
                        res += c;
                    }
                }
                return res;
            }
        };
    }
}

#endif
