#!/usr/bin/env python3

import sys


def main():
    replace_mapping = {
        "\n": "",
        "_ADJ": "",
        "_ADP": "",
        "_ADV": "",
        "_CONJ": "",
        "_DET": "",
        "_NOUN": "",
        "_NUM": "",
        "_PRON": "",
        "_PRT": "",
        "_VERB" :""
        }

    current_ngram = ""
    freq = 0
    for l in sys.stdin:
        for i in replace_mapping:
            l = l.replace(i, replace_mapping[i])
        if "_" not in l:
            tmp = l.split("\t")
            if tmp[0] == current_ngram:
                freq += int(tmp[-1])
            else:
                print("\t".join([current_ngram, str(freq)]))
                freq = 0
                current_ngram = tmp[0]

    #


if __name__ == "__main__":
    main()