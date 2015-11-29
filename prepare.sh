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

rocksdb() {
    if [ -d "rocksdb" ]; then
        logInfo "rocksdb already installed locally"
        return
    fi
    wget "https://github.com/facebook/rocksdb/archive/v4.1.tar.gz"
    tar -zxvf "v4.1.tar.gz"
    mv "rocksdb-4.1" "rocksdb"
    cd rocksdb
    make clean
    make static_lib -j 4
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

boost() {
    if [ -d "boost" ]; then
        logInfo "boost is already installed locally"
        return
    fi
    logInfo "boost download"
    wget -c "http://sourceforge.net/projects/boost/files/boost/1.59.0/boost_1_59_0.tar.bz2/download"
    mv "download" "boost_1_59_0.tar.bz2"
    tar -jxvf "boost_1_59_0.tar.bz2"
    mv "boost_1_59_0" "boost"
    cd boost
    mkdir build
    ./bootstrap.sh --prefix=./build
    ./b2 install
    cd ..
}

cppnetlib() {
    if [ -d "cpp-netlib" ]; then
        logInfo "cpp-netlib already installed locally"
        return
    fi
    logInfo "cpp-netlib download"
    wget -c "http://downloads.cpp-netlib.org/0.11.2/cpp-netlib-0.11.2-final.tar.bz2"
    tar -jxvf "cpp-netlib-0.11.2-final.tar.bz2"
    mv "cpp-netlib-0.11.2-final" "cpp-netlib"
    cd "cpp-netlib"
    mkdir build
    cd build
    cmake ../
    make -j 4
    cd ..
    cd ..
}

mkdir -p libs
cd libs
leveldb
#rocksdb
boost
#cppnetlib
cd ..

mkdir -p tools
cd tools
cpplint
cd ..