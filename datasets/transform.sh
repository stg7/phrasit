#!/bin/bash
#   Script for transforming one gz file to phrasit's import format
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

#
#   cat with automatically detection for compressed files
#
#   example call:
#       ccat filename.lzma
#    will print to stdout the uncompressed content of the file
#
ccat() {
    file="$1"
    filename="$(basename $file)"
    extension="${filename##*.}"
    if [ "$extension" = "lzma" ]; then
        lzcat "$file"
    fi
    if [ "$extension" = "gz" ]; then
        zcat "$file"
    fi
    if [ "$extension" = "bz2" ]; then
        bzcat "$file"
    fi
    cat "$file"
}

infile="$1"
filename="$(basename $infile)"
outdir="$(dirname $1)/transformed"
mkdir -p "$outdir"
outfile="$outdir/${filename%%.*}.lzma"
extension="${filename##*.}"

ccat "$infile" | ../utils/transform.py | lzma -9 --stdout > "$outfile"

