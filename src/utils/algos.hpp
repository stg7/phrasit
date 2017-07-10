/**
    part of phrasit

    \author stg7

    \brief algos module

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

#ifndef ALGOS_HEADER_HPP_
#define ALGOS_HEADER_HPP_

#include <vector>
#include <functional>
#include <algorithm>
#include <random>

#include "consts.hpp"
#include "utils/log.hpp"

namespace phrasit {
    namespace utils {

        template<typename T> inline T quick_select_rek(std::vector<T>& list, unsigned long left, unsigned long right, unsigned long k) {
            // based on: https://en.wikipedia.org/wiki/Quickselect
            if (left == right) {
                return list[left];
            }
            std::default_random_engine generator;
            std::uniform_int_distribution<unsigned long> distribution(left, right);

            unsigned long pivot = distribution(generator);
            static const auto partition = [&list](unsigned long left, unsigned long right, unsigned long pivot) -> unsigned long {
                T pivotValue = list[pivot];
                list[pivot] = list[right];
                list[right] = pivotValue;

                unsigned long store_index = left;

                for (unsigned long i = left; i < right; i++) {
                    if (list[i] < pivotValue) {
                        T tmp = list[store_index];
                        list[store_index] = list[i];
                        list[i] = tmp;
                        store_index ++;
                    }
                }
                T tmp = list[right];
                list[right] = list[store_index];
                list[store_index] = tmp;

                return store_index;
            };

            pivot = partition(left, right, pivot);
            if (k == pivot) {
                return list[pivot];
            }
            if (k < pivot) {
                return quick_select_rek(list, left, pivot - 1, k);
            }
            return quick_select_rek(list, pivot + 1, right, k);
        }

        template<typename T> inline T quick_select_iterativ(std::vector<T> list, unsigned long left, unsigned long right, unsigned long k) {
            // based on: https://en.wikipedia.org/wiki/Quickselect
            if (left > right) {
                return -1;
            }

            static const auto partition = [&list](unsigned long left, unsigned long right, unsigned long pivot) -> unsigned long {
                T pivotValue = list[pivot];
                list[pivot] = list[right];
                list[right] = pivotValue;

                unsigned long store_index = left;

                for (unsigned long i = left; i < right; i++) {
                    if (list[i] < pivotValue) {
                        T tmp = list[store_index];
                        list[store_index] = list[i];
                        list[i] = tmp;
                        store_index ++;
                    }
                }
                T tmp = list[right];
                list[right] = list[store_index];
                list[store_index] = tmp;

                return store_index;
            };

            while (left != right) {

                std::default_random_engine generator;
                std::uniform_int_distribution<unsigned long> distribution(left, right);

                unsigned long pivot = distribution(generator);

                pivot = partition(left, right, pivot);
                if (k == pivot) {
                    return list[pivot];
                }
                if (k < pivot) {
                    right = pivot - 1;
                } else {
                    left = pivot + 1;
                }
            }
            return list[left];
        }
        template<typename T> inline T quick_select(std::vector<T> list, unsigned long left, unsigned long right, unsigned long k) {
            return quick_select_iterativ<T>(list, left, right, k);
        }
    }
}
#endif