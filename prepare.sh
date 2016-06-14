#!/bin/bash
#   Script for downloading additional third-party libs
#
#   This file is part of PhrasIt.
#
#   PhrasIt is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   PhrasIt is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with PhrasIt.  If not, see <http://www.gnu.org/licenses/>.

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
    wget -c "http://sourceforge.net/projects/boost/files/boost/1.61.0/boost_1_61_0.tar.bz2/download"
    mv "download" "boost_1_61_0.tar.bz2"
    tar -jxvf "boost_1_61_0.tar.bz2"
    mv "boost_1_61_0" "boost"
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

    export BOOST_ROOT="$(pwd)/boost/build/"
    export BOOST_INCLUDEDIR="$(pwd)/boost/build/include"

    mv "cpp-netlib-0.11.2-final" "cpp-netlib"
    cd "cpp-netlib"
    mkdir build
    cd build
    cmake ../
    make -j 4
    mkdir -p lib
    find ./libs/ -name "*.a" -exec cp {} ./lib/ \;
    cd ..
    cd ..
}

cereal() {
    if [ -d "cereal" ]; then
        logInfo "cereal already installed locally"
        return
    fi
    wget "https://github.com/USCiLab/cereal/archive/v1.1.2.tar.gz"
    mv "v1.1.2.tar.gz" "cereal_v1.1.2.tar.gz"
    tar -zxvf "cereal_v1.1.2.tar.gz"
    mv "cereal-1.1.2" "cereal"
}

mkdir -p libs
cd libs
leveldb
boost
cppnetlib
cereal
cd ..

mkdir -p tools
cd tools
cpplint
cd ..