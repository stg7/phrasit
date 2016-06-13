/**
    part of phrasit

    \author stg7

    \brief caching module, LRU/LFR strategy

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
#include <functional>
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
            bool _initialized = false;
            bool _disabled = true;
            std::string _storagedir;
            std::map<K, unsigned long> _counts;
            std::map<K, V> _store;

         public:

            Cache(const unsigned long size): _size(size) {
                LOGINFO("create cache with " << size << " stored elements");
            }
            Cache(): Cache(1000) {
            }
            ~Cache() {
                LOGINFO("delete cache");
                if (!_initialized) {
                    return;
                }
                // store values in seperate files
                for (auto& k: _counts) {
                    LOGINFO(k.first << " -> " << k.second);
                }

            }

            void init(const std::string& storagebasedir, std::function<V (K)> do_query) {
                namespace fs = std::experimental::filesystem;
                _storagedir = storagebasedir + "/_cache";
                _initialized = true;
                if (!fs::exists(_storagedir)) {
                    fs::create_directory(_storagedir);
                    return;
                }
                std::vector<unsigned long> counts;
                std::ifstream counts_file(_storagedir + "/_counts");

                for (std::string line = ""; getline(counts_file, line);) {
                    counts.emplace_back(std::stoul(line));
                }
                counts_file.close();

                std::vector<std::string> keys;
                std::ifstream keys_file(_storagedir + "/_keys");

                for (std::string line = ""; getline(keys_file, line);) {
                    keys.emplace_back(line);
                }
                keys_file.close();

                utils::check(keys.size() == counts.size(), "size of counts and keys file does not match, fix it!");
                for(unsigned long i = 0; i < keys.size(); i++) {
                    _counts[keys[i]] = counts[i];
                    _store[keys[i]] = do_query(keys[i]);
                }
                _disabled = false;
            }

            V get(K key) {
                utils::check(has(key) == true, "key is not stored");
                _counts[key] ++;
                return _store[key];
            }

            const bool has(K key) const {
                if (_disabled) {
                    return false;
                }
                return _counts.find(key) != _counts.end();
            }

            bool put(const K key, const V& value) {
                LOGINFO("put " << key);
                if (has(key)) {
                    return false;
                }

                // if cache is full, do some cache cleaning
                if (_counts.size() == _size) {
                    LOGINFO("cleanup cache");
                    unsigned long min = _counts.begin()->second;
                    for(auto& x: _counts) {
                        if (x.second < min) {
                            min = x.second;
                        }
                    }
                    std::vector<K> to_delete;
                    for(auto& x: _counts) {
                        if (x.second <= min) {
                            to_delete.emplace_back(x.first);
                        }
                    }

                    for(auto& d: to_delete) {
                        _counts.erase(d);
                        _store.erase(d);
                    }

                }
                _counts[key] = 0;
                _store[key] = value;
                return true;
            }

        };
    }
}
#endif
