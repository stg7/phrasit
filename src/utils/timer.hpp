/**
    part of phraseit

    \author stg7

    \brief timer

    \date 13.11.2015

    Copyright 2015 Steve Göring
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