#!/bin/bash

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

