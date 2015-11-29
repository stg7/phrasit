#!/bin/bash
#
#    Printout info message.
#
logInfo() {
    echo -e "\033[92m[INFO ]\033[0m $@"
}


branch=$(git branch | grep "*" | sed "s|\* ||g")
version=$(git rev-parse HEAD)

version="$version @ $(date)"
logInfo "branch: $branch"
logInfo "version: $version"

echo "// build infos -- file will be created automatically" > src/build.hpp
echo "static constexpr const char* version=\"$version\";" >> src/build.hpp
echo "static constexpr const char* branch=\"$branch\";" >> src/build.hpp



