/**
    part of phrasit

    \author stg7

    \brief handle memory mapped files

    \date 18.11.2015

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

#ifndef MMFILES_HEADER_HPP_
#define MMFILES_HEADER_HPP_

#include <iostream>
#include <string>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>

#include "utils/helper.hpp"

namespace phrasit {
    namespace utils {
        namespace size {
            constexpr unsigned long kb(unsigned long bytes) {
                return bytes * 1024;
            }
            constexpr unsigned long mb(unsigned long bytes) {
                return kb(bytes) * 1024;
            }
            constexpr unsigned long gb(unsigned long bytes) {
                return mb(bytes) * 1024;
            }
        }

        template<class T = char> class MMArray;  // default template parameter for MMArray is char

        /**
        *   \brief simple memory mapped file based array structure as vector replacement
        *   \param T array element type (mapped values)
        *
        **/
        template <class T> class MMArray {
         private:
            boost::iostreams::mapped_file _file;  //< read write access to memory mapped file
            T* _data;  //< mapped data pointer of type T
            bool _is_opened = false;

         public:
            /*
            *   create not opened mmfile
            */
            MMArray() {}

            /*
            *   open file 'filename', if size != 0 create a new file
            */
            void open(const std::string filename, unsigned long size = 0) {
                namespace fs = boost::filesystem;
                boost::iostreams::mapped_file_params  params;
                params.path = filename;
                params.mode = (std::ios_base::out | std::ios_base::in);
                if (!fs::exists(filename)) {
                    check(size != 0, "if you open a not existing file, size must be > 0");
                    params.new_file_size = size;
                }

                _file.open(params);
                _data = reinterpret_cast<T *>(_file.data());
                _is_opened = true;
            }

            /*
            *   close mmfile
            */
            void close() {
                _file.close();
                _data = NULL;
            }

            bool is_open() {
                return _is_opened;
            }
            /*
            *   open memory mapped file
            *   \param filename
            *   \param size if size = 0, file must be existing
            */
            MMArray(const std::string filename, unsigned long size = 0) {
                open(filename, size);
            }

            ~MMArray() {
            }

            T get(const unsigned long i) const {
                check(i < size() && i >= 0, "get- index is not valid");
                return _data[i];
            }

            void set(const unsigned long i, T value) {
                check(i < size() && i >= 0, "set- index is not valid");
                _data[i] = value;
            }

            T& operator[] (const unsigned long i) {
                check(i < size() && i >= 0, "[] - index is not valid");
                return _data[i];
            }

            const T& operator[] (const unsigned long i) const {
                check(i < size() && i >= 0, "c[] - index is not valid");
                return _data[i];
            }

            size_t size() const {
                return _file.size() / sizeof(T);
            }
        };
    }
}
#endif
