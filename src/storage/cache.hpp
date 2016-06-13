/**
    part of phrasit

    \author stg7

    \brief caching module

    \date 13.06.2016

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

#ifndef CACHE_HEADER_HPP_
#define CACHE_HEADER_HPP_

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <experimental/filesystem>

#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "consts.hpp"

namespace phrasit {
    namespace storage {
        template<typename K, typename V>
        class Cache {
         private:
            unsigned long _size = 0;
            std::map<K, unsigned long> _counts;
            std::map<K, V> _store;

         public:

            Cache(const unsigned long size): _size(size) {
                LOGINFO("create cache with " << size << " stored elemens");
            }
            Cache(): Cache(1000) {
            }
            ~Cache() {
                LOGINFO("delete cache");
            }

            V get(K key) {
                utils::check(has(key) == true, "key is not stored");
                _counts[key] ++;
                return _store[key];
            }

            const bool has(K key) const {
                return _counts.find(key) != _counts.end();
            }

            bool put(const K key, const V& value) {
                LOGINFO("put " << key);
                if (has(key)) {
                    return false;
                }
                // TODO(stg7) add some magic
                //  so only insert size elements, then remove element with low count
                _counts[key] = 0;
                _store[key] = value;
                return true;
            }

        };
    }
}
#endif
