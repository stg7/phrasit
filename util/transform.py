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

    for l in sys.stdin:
        for i in replace_mapping:
            l = l.replace(i, replace_mapping[i])
        if "_" not in l:
            print(l.split("\t"))
    #


if __name__ == "__main__":
    main()