#!/bin/bash
infile="$1"
outfile="$(dirname $1)/$(basename $1 .gz).lzma"
zcat "$infile" | lzma -9 -c -T 0 --stdout > "$outfile"
#rm -rf "$infile"
