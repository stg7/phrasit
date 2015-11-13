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
#include <fstream>
#include <sstream>

#include "utils/log.hpp"
#include "utils/helper.hpp"


#include "leveldb/db.h"

void import(const std::vector<std::string>& ngram, const std::string& dbname) {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, dbname, &db);

    db->Put(leveldb::WriteOptions(), ngram[0], ngram[1]);
}

/**
    phrasit: command line client
**/
int main(int argc, const char* argv[]) {
    namespace po = boost::program_options;

    // declare the supported options.
    po::options_description desc("phraseit - an opensource netspeak clone\n\nSteve Göring 2015\nParameter");
    desc.add_options()
        ("help,h", "produce help message")
        ("storagedir,s", po::value<std::string>(), "storage directory, default='storage'")
        ("queryfile,f", po::value<std::string>(), "handle queries stored in a file")
        ("import,i", "import from stdin, format: ngram tab freq");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error& e) {
        LOGMSG("error: " << e.what());
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

    std::string queryfile = "";
    if (vm.count("queryfile") != 0) {
        queryfile = vm["queryfile"].as<std::string>();
    }

    LOGMSG("start phrasit");
    LOGMSG("using storagedir: " << storagedir);

    if (vm.count("import") != 0) {
        LOGMSG("import from stdin: ");
        char delimiter = '\t';
        std::string input_line;

        while(std::cin) {
            getline(std::cin, input_line);

            std::vector<std::string> x = phrasit::utils::split(input_line, delimiter);

            if (x.size() == 2) {
                std::cout << x[0] << "  " << x[1] << std::endl;
                import(x, storagedir + "/ids");
            }
        }
        return 0;
    }

    if (queryfile != "") {
        // run queries from a file
        LOGMSG("run queries from file: " << queryfile);
    } else {
        LOGMSG("run interactive mode ");

    }

    LOGINFO("end phrasit");
    return 0;
}