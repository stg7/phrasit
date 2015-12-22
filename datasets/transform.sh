#!/bin/bash

c_cat() {
    file="$1"
    filename="$(basename $file)"
    extension="${filename##*.}"
    if [ "$extension" = "lzma" ]; then
        lzcat "$file"
    fi
    if [ "$extension" = "gz" ]; then
        zcat "$file"
    fi
}

infile="$1"
filename="$(basename $infile)"
outdir="$(dirname $1)/transformed"
mkdir -p "$outdir"
outfile="$outdir/${filename%%.*}.lzma"
extension="${filename##*.}"

c_cat "$infile" | ../utils/transform.py | lzma -9 --stdout > "$outfile"

