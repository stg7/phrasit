/**
    part of phrasit

    \author stg7

    \brief constexpr hash functions

    \date 04.09.2014

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

#ifndef CONST_HASH_HEADER_HPP_
#define CONST_HASH_HEADER_HPP_

#include <string>


namespace phrasit {
    namespace utils {
        // copied from : http://seanmiddleditch.com/journal/2011/05/compile-time-string-hashing-in-c0x/
        // NOTE: hashing algorithm used is FNV-1a see: http://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

        // FNV-1a constants
        static constexpr uint64_t basis = 14695981039346656037ULL;
        static constexpr uint64_t prime = 1099511628211ULL;

        // compile-time hash helper function
        static constexpr uint64_t hash_one(char c, const char* remain, uint64_t value) {
            return c == 0 ? value : hash_one(remain[0], remain + 1, (value ^ c) * prime);
        }

        // compile-time hash
        static constexpr uint64_t c_hash(const char* str) {
            return hash_one(str[0], str + 1, basis);
        }

        // run-time hash with len parameter
        static uint64_t r_hash(const char* str, const size_t len) {
            uint64_t hash = basis;
            size_t i = 0;
            while (*str != 0 && i < len) {
                hash ^= str[0];
                hash *= prime;
                str++;
                i++;
            }
            return hash;
        }

        // run-time hash function for std::string
        static uint64_t r_hash(const std::string& s) {
            return r_hash(s.c_str(), s.length());
        }

    }
}

#endif
