#!/bin/bash

leveldb() {
    if [ -d "leveldb" ]; then
        echo "leveldb already installed locally"
        return
    fi
    echo "leveldb download and compiling"
    git clone https://github.com/google/leveldb.git
    cd leveldb
    make -j 4
    cd ..
    echo "leveldb done."
}
mongoose() {
    if [ -d "mongoose-cpp" ]; then
        echo "mongoose-cpp already installed locally"
        return
    fi
    echo "mongoose-cpp download and compiling"

    git clone https://github.com/Gregwar/mongoose-cpp.git
    cd mongoose-cpp
    cmake CMakeLists.txt
    make -j 4
    cd ..
}

cpplint() {
    if [ -f "cpplint.py" ]; then
        echo "cpplint.py already installed locally"
        return
    fi
    echo "cpplint.py download"
    wget "https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py"
}

cd libs
leveldb
mongoose

mkdir -p tools
cd tools
cpplint
