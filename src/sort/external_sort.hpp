/**
    part of phrasit

    \author stg7

    \brief external sort algo

    \date 16.11.2015

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

#ifndef EXTERNAL_SORT_HEADER_HPP_
#define EXTERNAL_SORT_HEADER_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <deque>

#include <cstdio>

#include <boost/filesystem.hpp>

#include "sort/parallel_sort.hpp"
#include "utils/log.hpp"

namespace phrasit {
    namespace sort {

        /*
        *   external sorting of a textfile
        */
        inline std::string external_sort(const std::string& filename, const std::string& tmppath, const long blocksize = 500000000) {
            LOGDEBUG("external sort");
            namespace fs = boost::filesystem;

            // read file line by line and store it in blockfiles that are sorted
            std::ifstream file(filename);
            std::string line = "";
            std::vector<std::string> lines;
            long block = 0;

            LOGDEBUG("split files");
            std::deque<std::string> blockfilenames;
            long readed_bytes = 0;
            std::ofstream out;

            while (!file.eof()) {
                getline(file, line);
                if (line == "") {
                    continue;
                }
                lines.emplace_back(line);
                readed_bytes += line.length();

                if (readed_bytes >= blocksize) {
                    readed_bytes = 0;
                    LOGDEBUG("parallel sort");

                    parallel_sort(lines);

                    std::string blockfilename = tmppath + "/" + std::to_string(block);
                    out.open(blockfilename);
                    if (!out.is_open()) {
                        LOGERROR("there is something wrong with outputfile " << blockfilename);
                        std::perror("is open failed:");
                    }
                    out.sync_with_stdio(false);
                    blockfilenames.push_front(blockfilename);

                    LOGDEBUG("write sorted lines to file: " << blockfilename);
                    for (auto& l : lines) {
                        out << l << "\n";
                        if (out.bad()) {
                            LOGERROR("there is something wrong with appendling line " << l);
                            std::perror("appending line failed:");
                        }
                    }
                    out.close();
                    lines.clear();
                    block++;
                }
            }

            block++;
            // handle remaining lines
            std::string blockfilename = tmppath + "/" + std::to_string(block);
            out.open(blockfilename);
            if (!out.is_open()) {
                LOGERROR("there is something wrong with outputfile " << blockfilename);
                std::perror("is open failed:");
            }
            out.sync_with_stdio(false);
            blockfilenames.push_front(blockfilename);

            LOGDEBUG("parallel sort");
            parallel_sort(lines);
            for (auto& l : lines) {
                out << l << "\n";
                if (out.bad()) {
                    LOGERROR("there is something wrong with appendling line " << l);
                    std::perror("appending line failed:");
                }
            }
            out.close();
            lines.clear();

            // now we can merge each block stepwise, let us start from the last block
            block++;
            std::ifstream block1;
            std::ifstream block2;
            std::ofstream b_merged;

            while (blockfilenames.size() > 1) {
                std::string b1 = blockfilenames.back();
                blockfilenames.pop_back();
                std::string b2 = blockfilenames.back();
                blockfilenames.pop_back();
                LOGDEBUG("merging blocks: " << b1 << " " << b2 << " to " << block);

                block1.open(b1);
                block2.open(b2);

                std::string bm = tmppath + "/" + std::to_string(block);
                block++;

                blockfilenames.push_front(bm);

                std::string l1 = "";
                std::string l2 = "";
                b_merged.open(bm);
                b_merged.sync_with_stdio(false);
                getline(block1, l1);
                getline(block2, l2);

                while (!block1.eof() && !block2.eof() && l1 != "" && l2 != "") {
                    if (l1 < l2) {
                        // write l1 to result file
                        b_merged << l1 << "\n";
                        getline(block1, l1);
                    } else {
                        // write l2 to result file
                        b_merged << l2 << "\n";
                        getline(block2, l2);
                    }
                }
                while (!block1.eof()) {
                    if (l1 != "") {
                        // write l1 to result file
                        b_merged << l1 << "\n";
                    }
                    getline(block1, l1);
                }
                if (l1 != "") {
                    // write l1 to result file
                    b_merged << l1 << "\n";
                }
                while (!block2.eof()) {
                    if (l2 != "") {
                        // write l2 to result file
                        b_merged << l2 << "\n";
                    }
                    getline(block2, l2);
                }
                if (l2 != "") {
                    // write l2 to result file
                    b_merged << l2 << "\n";
                }

                b_merged.close();
                block1.close();
                block2.close();

                // tidy up temporary files
                LOGDEBUG("delete files: " << b1 << " " << b2);
                if (fs::exists(b1)) {
                    fs::remove(b1);
                }
                if (fs::exists(b2)) {
                    fs::remove(b2);
                }
            }

            LOGDEBUG("results are stored in: " << blockfilenames.back());
            return blockfilenames.back();
        }
    }
}
#endif
