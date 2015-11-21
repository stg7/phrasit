/**
    part of phraseit

    \author stg7

    \brief progress bar

    \date 21.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef PROGRESS_BAR_HEADER_HPP_
#define PROGRESS_BAR_HEADER_HPP_

#include <iostream>

namespace phrasit {
    namespace utils {

        /*
        *   progress bar
        *
        */
        class Progress_bar {
         private:
            const char _wait_char_first = '.';
            const char _wait_char_second = '_';
            const int _cols = 60;

            char _wait_char;
            unsigned long _glob_count = 0;
            unsigned long _count = 0;
            unsigned long _update_count;

         public:
            Progress_bar(unsigned long update_count = 1): _wait_char(_wait_char_first), _update_count(update_count) {

            }
            ~Progress_bar() {
                std::cout << std::endl;
            }

            void update() {
                if(_count %  _update_count == 0) {
                    if (_glob_count % _cols == 0) {
                        std::cout << "\r" << _glob_count / 60;
                        if (_wait_char == _wait_char_first) {
                            _wait_char = _wait_char_second;
                        } else {
                            _wait_char = _wait_char_first;
                        }
                    }
                    _glob_count ++;

                    std::cout << _wait_char;
                    std::cout.flush();
                }
                _count ++;
            }
        };
    }
}
#endif
