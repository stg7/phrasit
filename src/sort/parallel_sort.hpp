/**
    part of phrasit

    \author stg7

    \brief modified parallel sort implementation, based on https://github.com/sol-prog/Sort_data_parallel

    parallel sort implementation
    description:
        https://solarianprogrammer.com/2013/02/04/sorting-data-in-parallel-cpu-gpu/

    \date 30.09.2014

    Copyright 2015 Steve Göring
**/

#ifndef PARALLEL_SORT_HEADER_HPP_
#define PARALLEL_SORT_HEADER_HPP_

#include <algorithm>
#include <thread>
#include <functional>
#include <vector>

#include "utils/log.hpp"

namespace phrasit {
    namespace sort {

        template<typename T>
        inline void parallel_sort(std::vector<T>& V, long parts = -1) {
            if (parts == -1) {  // default case: use all aviable cpu cores
                parts = std::thread::hardware_concurrency();
            }
            if (V.size() - parts <= 0) {
                std::sort(std::begin(V), std::begin(V));
            }

            // split the data in "parts" pieces and sort each piece in a separate thread
            std::vector<long> bnd(parts + 1);
            long delta = V.size() / parts;
            long reminder = V.size() % parts;
            long N1 = 0, N2 = 0;
            bnd[0] = N1;
            for (long i = 0; i < parts; ++i) {
                N2 = N1 + delta;
                if (i == parts - 1)
                    N2 += reminder;
                bnd[i + 1] = N2;
                N1 = N2;
            }

            std::vector<std::thread> thr;
            thr.reserve(parts);

            // create threads for each part and sort parts
            for (long i = 0; i < parts; i++) {
                thr.emplace_back(
                    std::thread(
                        [](std::vector<T>& V, const long left, const long right) {
                            std::sort(std::begin(V) + left, std::begin(V) + right);
                        }, std::ref(V), bnd[i], bnd[i + 1]));
            }

            for (auto& t : thr) {
                t.join();
            }
            thr.clear();


            std::vector<long> limits;
            limits.reserve(parts);

            // merge two parts together

            while (parts >= 2) {
                for (long i = 0; i < parts - 1; i += 2) {
                    thr.emplace_back(
                        std::thread(
                            [](std::vector<T>& V, long n0, long n1, long n2, long n3) {
                                std::inplace_merge(std::begin(V) + n0, std::begin(V) + n1, std::begin(V) + n3);
                            }, std::ref(V), bnd[i], bnd[i + 1], bnd[i + 1], bnd[i + 2]));

                    long naux = limits.size();
                    if (naux > 0) {
                        if (limits[naux - 1] != bnd[i]) {
                            limits.emplace_back(bnd[i]);
                        }
                        limits.emplace_back(bnd[i + 2]);
                    } else {
                        limits.emplace_back(bnd[i]);
                        limits.emplace_back(bnd[i + 2]);
                    }
                }

                for (auto& t : thr) {
                    t.join();
                }

                parts /= 2;
                bnd = limits;
                limits.clear();
                thr.clear();
            }
        }
    }
}
#endif
