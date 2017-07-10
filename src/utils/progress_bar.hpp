/**
    part of phraseit

    \author stg7

    \brief progress bar

    \date 21.11.2015

    Copyright 2017 Steve Göring

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

#ifndef PROGRESS_BAR_HEADER_HPP_
#define PROGRESS_BAR_HEADER_HPP_

#include <iostream>
#include <string>

namespace phrasit {
    namespace utils {

        /*
        *   progress bar
        */
        class Progress_bar {
         private:
            const char _wait_char_first = '.';
            const char _wait_char_second = '-';
            const int _cols = 60;

            char _wait_char;
            unsigned long _glob_count = 0;
            unsigned long _count = 0;
            unsigned long _update_count;
            std::string _msg;

         public:
            Progress_bar(unsigned long update_count = 1, const std::string& msg = ""):
                _wait_char(_wait_char_first),
                _update_count(update_count),
                _msg(msg) { }

            ~Progress_bar() {
                std::cout << std::endl;
            }

            /*
            *   perform an update step and do output on stdout
            */
            void update() {
                if (_count %  _update_count == 0) {
                    if (_glob_count % _cols == 0) {
                        std::cout << "\r" << "\033[1;94m[" << _msg << "]\033[0m " << _glob_count / _cols << " ";
                        if (_wait_char == _wait_char_first) {
                            _wait_char = _wait_char_second;
                        } else {
                            _wait_char = _wait_char_first;
                        }
                    }
                    _glob_count++;
                    std::cout << _wait_char;
                    std::cout.flush();
                }
                _count++;
            }
        };
    }
}
#endif
