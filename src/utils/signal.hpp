/**
    part of phrasit

    \author stg7

    \brief signal handling

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

#ifndef SIGNAL_HEADER_HPP_
#define SIGNAL_HEADER_HPP_

#include <csignal>

namespace phrasit {
    namespace utils {
        static int _s_interrupted = 0;
        static inline void __s_signal_handler(int signal_value) {
            _s_interrupted = 1;
        }

        static inline bool control_c_pressed() {
            return _s_interrupted == 1;
        }

        static inline void signal_init() {
            struct sigaction action;
            action.sa_handler = __s_signal_handler;
            action.sa_flags = 0;
            sigemptyset(&action.sa_mask);
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGTERM, &action, NULL);
        }
    }
}
#endif