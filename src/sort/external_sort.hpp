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

#include "compress/file.hpp"
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
            compress::File<compress::mode::read> file(filename);

            std::vector<std::string> lines;
            long block = 0;

            LOGDEBUG("split files");
            std::deque<std::string> blockfilenames;
            long readed_bytes = 0;

            auto write_to_outfile = [&]() {
                LOGDEBUG("parallel sort");
                parallel_sort(lines);
                std::string blockfilename = tmppath + "/" + std::to_string(block) + ".gz";
                compress::File<compress::mode::write> out;

                out.open(blockfilename);

                if (!out.is_open()) {
                    LOGERROR("there is something wrong with outputfile " << blockfilename);
                    std::perror("is open failed:");
                }

                blockfilenames.push_front(blockfilename);
                for (auto& l : lines) {
                    out.writeLine(l);
                }
                out.close();
                lines.clear();
            };

            for(std::string line = ""; file.readLine(line); ) {
                lines.emplace_back(line);
                readed_bytes += line.length();
                if (readed_bytes >= blocksize) {
                    readed_bytes = 0;
                    write_to_outfile();
                    block++;
                }
            }

            // handle remaining lines
            write_to_outfile();

            // now we can merge each block stepwise, let us start from the last block
            block++;
            compress::File<compress::mode::read> left;
            compress::File<compress::mode::read> right;
            compress::File<compress::mode::write> merged;


            while (blockfilenames.size() > 1) {
                std::string left_filename = blockfilenames.back();
                blockfilenames.pop_back();
                std::string right_filename = blockfilenames.back();
                blockfilenames.pop_back();

                std::string merged_filename = tmppath + "/" + std::to_string(block) + ".gz";
                LOGDEBUG("merging blocks: " << left_filename << " "
                    << right_filename << " to " << merged_filename);

                left.open(left_filename);
                right.open(right_filename);

                blockfilenames.push_front(merged_filename);
                block++;

                std::string l1 = "";
                std::string l2 = "";
                merged.open(merged_filename);

                bool r1 = left.readLine(l1);
                bool r2 = right.readLine(l2);
                while (r1 && r2) {
                    if (l1 < l2) {
                        merged.writeLine(l1);
                        r1 = left.readLine(l1);
                    } else {
                        merged.writeLine(l2);
                        r2 = right.readLine(l2);
                    }
                }

                while (r1) {
                    merged.writeLine(l1);
                    r1 = left.readLine(l1);
                }

                if (l1 != "") {
                    merged.writeLine(l1);
                }

                while (r2) {
                    merged.writeLine(l2);
                    r2 = right.readLine(l2);
                }
                if (l2 != "") {
                    merged.writeLine(l2);
                }

                merged.close();
                left.close();
                right.close();

                // tidy up temporary files
                LOGDEBUG("delete files: " << left_filename << " " << right_filename);
                if (fs::exists(left_filename)) {
                    fs::remove(left_filename);
                }
                if (fs::exists(right_filename)) {
                    fs::remove(right_filename);
                }
            }

            LOGDEBUG("results are stored in: " << blockfilenames.back());
            return blockfilenames.back();
        }
    }
}
#endif
