#!/bin/bash
#   Script for adding build infos to phrasit
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
echo "static constexpr const char* version = \"$version\";" >> src/build.hpp
echo "static constexpr const char* branch = \"$branch\";" >> src/build.hpp



