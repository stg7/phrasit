#!/usr/bin/env python3

import os
import sys

def main(params):

    histo = {}

    for l in sys.stdin:
        l = l[0:-1]
        tmp = l.split("\t")[0]
        for n in range(2, 5):
            for i in range(0, n * (len(tmp) // n), n):
                ngram = tmp[i:i + n]
                histo[ngram] = 1 + histo.get(ngram, 0)

    top = sorted(histo.items(), key=lambda x:x[1] * len(x[0]), reverse=True)[0:31]

    # from 0..32 non printable chars, 0 is reserverd for string end \0
    # therefore 1..32 can be used as compressing symbols

    comp_table = {}

    # "std::string arr[256] = {"

    j = 1
    for k, v in sorted(top, key=lambda x: len(x[0]), reverse=True):
        comp_table[j] = k
        j += 1

    for i in range(256):
        if i not in comp_table:
            comp_table [i] = chr(i)

    print(comp_table)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))