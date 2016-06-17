/**
    part of phrasit

    \author stg7

    \brief main file

    \date 13.11.2015

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

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <thread>
#include <experimental/filesystem>

#include "cxxopts.hpp"

#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "utils/timer.hpp"
#include "utils/progress_bar.hpp"
#include "utils/signal.hpp"
#include "srv/webserver.hpp"
#include "phrasit.hpp"
#include "consts.hpp"

void print_search_results(const std::string& query, phrasit::Phrasit& phrasit) {
    const auto& results = phrasit.search(query);
    LOGINFO("got " << results.size() << " results...");
    for (const auto& res : results) {
        std::cout << phrasit.get_ngram(res) << "  : " << phrasit.get_freq(res) << std::endl;
    }
}

/**
    phrasit: command line client
**/
int main(int argc, char* argv[]) {
    namespace fs = std::experimental::filesystem;

    cxxopts::Options options(argv[0], " - an opensource netspeak clone\n\nSteve Göring 2016\nParameter");
    options.add_options()
        ("h,help", "produce help message")
        ("d,storagedir", "storage directory, default='storage'", cxxopts::value<std::string>())
        ("f,queryfile", "handle queries stored in a file", cxxopts::value<std::string>())
        ("i,import", "import from stdin, format: ngram tab freq")
        ("o,optimize", "optimize index (e.g. if import failed with optimisation)")
        ("max-res", "maximum result size", cxxopts::value<unsigned long>())
        ("s,server", "start phrasit in server mode");

    try {
        options.parse(argc, argv);
    } catch (...) {
        LOGERROR("error wrong arguments");
        std::cout << options.help() << std::endl;
        return -1;
    }

    if (options.count("help")) {
      std::cout << options.help() << std::endl;
      return -1;
    }

    if (options.count("max-res") != 0) {
        phrasit::max_result_size = options["max-res"].as<unsigned long>();
    }

    std::string storagedir = "storage";
    if (options.count("storagedir") != 0) {
        storagedir = options["storagedir"].as<std::string>();
    }

    if (!fs::exists(storagedir)) {
        LOGMSG("create storagedir: " << storagedir);
        fs::create_directory(storagedir);
    }

    std::string queryfilename = "";
    if (options.count("queryfile") != 0) {
        queryfilename = options["queryfile"].as<std::string>();
    }

    LOGMSG("start phrasit");
    LOGMSG("version: " << phrasit::version);
    LOGMSG("branch: " << phrasit::branch);

    LOGMSG("using storagedir: " << storagedir);

    phrasit::Phrasit phrasit(storagedir);

    if (options.count("server") != 0) {
        LOGMSG("server mode: ");
        phrasit::srv::Webserver webserver(phrasit, storagedir);
        webserver.start();
        return 0;
    }

    if (options.count("import") != 0) {
        LOGMSG("import from stdin: ");
        char delimiter = '\t';

        std::string input_line;

        phrasit::utils::Timer t;

        long ngram_count = 0;

        {
            phrasit::utils::Progress_bar pb(1000, "import");

            while (std::cin) {
                getline(std::cin, input_line);

                std::vector<std::string> splitted_line = phrasit::utils::split(input_line, delimiter);

                if (splitted_line.size() == 2) {
                    phrasit.insert(splitted_line[0], splitted_line[1]);
                    ngram_count++;
                    pb.update();
                }
            }
        }

        phrasit.optimize();

        LOGDEBUG("needed time: " << t.time() << " ms");
        LOGMSG("successfully imported " << ngram_count << " ngrams");
        return 0;
    } else {
        if (options.count("optimize") != 0) {
            LOGMSG("optimize index");
            phrasit.optimize();
            LOGMSG("successfully optimized index");
            return 0;
        }
    }

    if (queryfilename != "") {
        // run queries from a file
        LOGMSG("run queries from file: " << queryfilename);
        std::ifstream queryfile(queryfilename);
        phrasit::utils::Timer t;

        for (std::string query = ""; getline(queryfile, query);) {
            std::cout << "results of query: " << query << std::endl;
            print_search_results(query, phrasit);
        }

        LOGDEBUG("needed time: " << t.time() << " ms");
    } else {
        LOGMSG("run interactive mode ");

        phrasit::utils::signal_init();

        std::string query = "";
        do {
            std::cout << "please enter your query, ("
                << phrasit::exit_str << " or ctrl + d for exit): " << std::endl
                << phrasit::shell_str;
            getline(std::cin, query);
            phrasit::utils::Timer t;

            if (query != phrasit::exit_str && query != "") {
                print_search_results(query, phrasit);
            }

            LOGDEBUG("needed time: " << t.time() << " ms");
        } while (query != phrasit::exit_str && !std::cin.eof() && !phrasit::utils::control_c_pressed());

        std::cout << std::endl;
    }

    LOGINFO("end phrasit");
    return 0;
}
