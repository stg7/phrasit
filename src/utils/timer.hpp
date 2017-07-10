/**
    part of phraseit

    \author stg7

    \brief timer

    \date 13.11.2015

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

#ifndef TIMER_HEADER_HPP_
#define TIMER_HEADER_HPP_

#include <ctime>

namespace phrasit {
    namespace utils {
        /*
        *   timer class for measuring cpu-tick counts in ms
        *
        *   timer automatically starts during construction
        *   and stops if time() method is called.
        */
        class Timer {
         private:
            std::clock_t _start;
            std::clock_t _end;
         public:
            Timer() {
                start();
            }

            void start() {
                 _start = std::clock();
            }

            void stop() {
                _end = std::clock();
            }

            /*
            *   \return measured time in ms
            */
            double time() {
                stop();
                return (1000.0 * (_end - _start)) / CLOCKS_PER_SEC;
            }
        };
    }
}
#endif
