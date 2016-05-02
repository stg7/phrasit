/**
    part of phrasit

    \author stg7

    \brief compressed file classes for reading and writing

    \date 06.02.2016

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

#ifndef FILE_HEADER_HPP_
#define FILE_HEADER_HPP_

#include <fstream>
#include <iostream>
#include <string>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

namespace phrasit {
    namespace compress {

        namespace mode {
            struct write {};
            struct read {};
        }

        // wrapper for gzipped files based on boost iostreams
        //  File<mode:write> write
        // or
        //  File<mode:read> read

        template <class T> class File;

        template <>
        class File<mode::write> {
         private:
            boost::iostreams::filtering_ostream _out;
            std::ofstream _file;

         public:
            File() { }

            File(const std::string& filename) {
                open(filename);
            }

            void open(const std::string& filename) {
                _file.open(filename, std::ios_base::app | std::ios_base::binary);
                _file.sync_with_stdio(false);
                _out.push(boost::iostreams::gzip_compressor(
                    boost::iostreams::gzip_params(
                        boost::iostreams::gzip::best_speed)));
                    // best_compression is just a bit better but needs double time
                _out.push(_file);
            }

            ~File() {
                close();
            }

            inline bool is_open() {
                return _file.is_open();
            }

            inline void close() {
                if (is_open()) {
                    _out.reset();
                    boost::iostreams::close(_out);
                    _file.close();
                }
            }

            inline void writeLine(const std::string& l) {
                _out << l << "\n";
            }
        };

        template <>
        class File<mode::read> {
         private:
            boost::iostreams::filtering_istream _in;
            std::ifstream _file;

         public:
            File() { }

            File(const std::string& filename) {
                open(filename);
            }

            void open(const std::string& filename) {
                _file.open(filename, std::ios_base::in | std::ios_base::binary);
                _file.sync_with_stdio(false);
                _in.push(boost::iostreams::gzip_decompressor());
                _in.push(_file);
            }

            ~File() {
                close();
            }

            inline bool is_open() {
                return _file.is_open();
            }

            inline void close() {
                if (is_open()) {
                    _in.reset();
                    boost::iostreams::close(_in);
                    _file.close();
                }
            }

            inline bool readLine(std::string& str) {
                return std::getline(_in, str).good();
            }
        };

    }
}

#endif