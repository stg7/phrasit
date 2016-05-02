/**
    part of phrasit

    \author stg7

    \brief memory module (reading free memory)

    \date 06.02.2016

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

#ifndef MEMORY_HEADER_HPP_
#define MEMORY_HEADER_HPP_

#include <fstream>
#include <limits>
#include <string>

namespace phrasit {
    namespace utils {
        namespace memory {

            // based on: http://stackoverflow.com/questions/349889/how-do-you-determine-the-amount-of-linux-system-ram-in-c

            // get total memory in kB
            unsigned long get_total() {
                std::string token;
                std::ifstream file("/proc/meminfo");
                while(file >> token) {
                    if(token == "MemTotal:") {
                        unsigned long mem;
                        if(file >> mem) {
                            return mem;
                        } else {
                            return 0;
                        }
                    }
                    // ignore rest of the line
                    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                return 0; // nothing found
            }

            // get free memory in kB
            unsigned long get_free() {
                std::string token;
                std::ifstream file("/proc/meminfo");
                while(file >> token) {
                    if(token == "MemAvailable:") {
                        unsigned long mem;
                        if(file >> mem) {
                            return mem;
                        } else {
                            return 0;
                        }
                    }
                    // ignore rest of the line
                    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                return 0; // nothing found
            }
        }
    }
}
#endif
