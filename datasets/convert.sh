#!/bin/bash
#   Script for converting one gz file to lzma file for better space utilisation
#   use it in combination with ./do_parallel.py
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


infile="$1"
if [ -f "$infile" ] ; then
    outfile="$(dirname $1)/$(basename $1 .gz).lzma"
    zcat "$infile" | lzma -9 -c -T 0 --stdout > "$outfile"
    if [ $? -eq 0 ]; then
        rm -rf "$infile"
    fi
fi
