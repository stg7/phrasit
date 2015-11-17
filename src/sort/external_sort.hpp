/**
    part of phrasit

    \author stg7

    \brief external sort algo

    \date 16.11.2015

    Copyright 2015 Steve Göring
**/

#ifndef EXTERNAL_SORT_HEADER_HPP_
#define EXTERNAL_SORT_HEADER_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "sort/parallel_sort.hpp"
#include "utils/log.hpp"

namespace phrasit {
    namespace sort {

        /*
        *   external sorting of a textfile
        */
        inline void external_sort(const std::string& filename, const std::string& tmppath, const long blocksize=100000000) {
            LOGDEBUG("external sort");
            // read file line by line in blockfiles that are sorted
            std::ifstream file(filename);
            std::string line = "";
            std::vector<std::string> lines;
            long block = 0;

            std::vector<std::string> blockfilenames;
            long readed_bytes = 0;
            while (!file.eof()) {
                getline(file, line);
                lines.emplace_back(line);
                readed_bytes += line.length();

                if (readed_bytes >= blocksize) {
                    readed_bytes = 0;

                    parallel_sort(lines);

                    std::string blockfilename = tmppath + "/" + std::to_string(block);
                    std::ofstream out(blockfilename);
                    blockfilenames.push_back(blockfilename);
                    for (auto& l : lines) {
                        out << l << "\n";
                    }
                    out.close();
                    lines.clear();
                    block++;
                }
            }

            block++;
            // handle remaining lines
            std::string blockfilename = tmppath + "/" + std::to_string(block);
            std::ofstream out(blockfilename);
            blockfilenames.push_back(blockfilename);

            for (auto& line : lines) {
                out << line << "\n";
            }
            out.close();

            // now we can merge each block in a step let us start from the end
            block++;

            while (blockfilenames.size() > 1) {
                std::string b1 = blockfilenames.back();
                blockfilenames.pop_back();
                std::string b2 = blockfilenames.back();
                blockfilenames.pop_back();
                LOGDEBUG("merging blocks: " << b1 << " " << b2 << " to " << block);

                std::ifstream block1(b1);
                std::ifstream block2(b2);

                std::string bm = tmppath + "/" + std::to_string(block);
                block++;

                blockfilenames.emplace_back(bm);

                std::string l1 = "";
                std::string l2 = "";
                std::ofstream b_merged(bm);

                getline(block1, l1);
                getline(block2, l2);

                while (!block1.eof() && !block2.eof()) {

                    if(l1 < l2) {
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
                    // write l1 to result file
                    b_merged << l1 << "\n";
                    getline(block1, l1);
                }
                // write l1 to result file
                b_merged << l1 << "\n";

                while (!block2.eof()) {
                    // write l2 to result file
                    b_merged << l2 << "\n";
                    getline(block2, l2);
                }
                // write l2 to result file
                b_merged << l2 << "\n";

                b_merged.close();

                // tidy up temporary files
                LOGDEBUG("delete files: " << b1 << " " << b2);
                if (boost::filesystem::exists(b1)) {
                    boost::filesystem::remove(b1);
                }
                if (boost::filesystem::exists(b2)) {
                    boost::filesystem::remove(b2);
                }
            }

            LOGDEBUG("results are stored in: " << blockfilenames.back());
        }
    }
}
#endif
