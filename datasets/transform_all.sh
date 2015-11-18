#!/bin/bash

mkdir -p transformed

ngrams="\
1gram
2gram
3gram
4gram
5gram
"
ngrams="\
2gram
"
for j in $ngrams; do
    outfile="transformed/$j"
    echo -n "" > "$outfile"
    for i in `ls *-$j*.gz`; do
        echo "handle $i"
        zcat "$i" | ./../utils/transform.py  >> "$outfile"
        echo "."
    done
done
