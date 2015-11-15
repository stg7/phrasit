#!/bin/bash

#
#    Printout error message.
#
logError() {
    echo -e "\033[91m[ERROR]\033[0m $@ " 1>&2;
}

#
#    Printout info message.
#
logInfo() {
    echo -e "\033[92m[INFO ]\033[0m $@"
}

leveldb() {
    if [ -d "leveldb" ]; then
        logInfo "leveldb already installed locally"
        return
    fi
    logInfo "leveldb download and compiling"
    git clone https://github.com/google/leveldb.git
    cd leveldb
    make -j 4
    cd ..
    logInfo "leveldb done."
}

mongoose() {
    if [ -d "mongoose-cpp" ]; then
        logInfo "mongoose-cpp already installed locally"
        return
    fi
    logInfo "mongoose-cpp download and compiling"

    git clone https://github.com/Gregwar/mongoose-cpp.git
    cd mongoose-cpp
    cmake CMakeLists.txt
    make -j 4
    cd ..
}

cpplint() {
    if [ -f "cpplint.py" ]; then
        logInfo "cpplint.py already installed locally"
        return
    fi
    logInfo "cpplint.py download"
    wget "https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py"
}

mkdir -p libs
cd libs
leveldb
mongoose
cd ..

mkdir -p tools
cd tools
cpplint
cd ..