/**
    part of phraseit

    \author stg7

    \brief main file

    \date 13.11.2015

    Copyright 2015 Steve Göring
**/
#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "boost/filesystem.hpp"

#include "utils/log.hpp"
#include "utils/helper.hpp"
#include "utils/timer.hpp"
#include "utils/progress_bar.hpp"
#include "srv/webserver.hpp"
#include "phrasit.hpp"
#include "consts.hpp"

void print_search_results(const std::string& query, phrasit::Phrasit& phrasit) {
    for (auto& res : phrasit.search(query)) {
        std::cout << res << std::endl;
    }
}

/**
    phrasit: command line client
**/
int main(int argc, const char* argv[]) {
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    // declare the supported options.
    po::options_description desc("phraseit - an opensource netspeak clone\n\nSteve Göring 2015\nParameter");
    desc.add_options()
        ("help,h", "produce help message")
        ("storagedir,d", po::value<std::string>(), "storage directory, default='storage'")
        ("queryfile,f", po::value<std::string>(), "handle queries stored in a file")
        ("import,i", "import from stdin, format: ngram tab freq")
        ("server,s", "start phrasit in server mode")
        ("opt,o", "optimize [debug]");;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error& e) {
        LOGERROR("error: " << e.what());
        std::cout << desc << std::endl;
        return -1;
    }

    if (vm.count("help") != 0) {
        std::cout << desc << std::endl;
        return -1;
    }

    std::string storagedir = "storage";
    if (vm.count("storagedir") != 0) {
        storagedir = vm["storagedir"].as<std::string>();
    }

    if (!fs::exists(storagedir)) {
        LOGMSG("create storagedir: " << storagedir);
        fs::create_directory(storagedir);
    }

    std::string queryfilename = "";
    if (vm.count("queryfile") != 0) {
        queryfilename = vm["queryfile"].as<std::string>();
    }

    LOGMSG("start phrasit");
    LOGMSG("version: " << phrasit::version);
    LOGMSG("branch: " << phrasit::branch);


    LOGMSG("using storagedir: " << storagedir);

    phrasit::Phrasit phrasit(storagedir);

    if (vm.count("server") != 0) {
        LOGMSG("server mode: ");
        phrasit::srv::Webserver webserver(phrasit);
        webserver.start();
        return 0;
    }
    if (vm.count("opt") != 0) {
        LOGMSG("optimize index: ");

        phrasit.optimize(true);
        return 0;
    }
    if (vm.count("import") != 0) {
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

        std::string query = "";
        do {
            std::cout << "please enter your query, ("
                << phrasit::exit_str << " or ctrl + d for exit): " << std::endl
                << phrasit::shell_str;
            getline(std::cin, query);
            phrasit::utils::Timer t;

            print_search_results(query, phrasit);

            LOGDEBUG("needed time: " << t.time() << " ms");
        } while (query != phrasit::exit_str && !std::cin.eof());

        std::cout << std::endl;
    }

    LOGINFO("end phrasit");
    return 0;
}
