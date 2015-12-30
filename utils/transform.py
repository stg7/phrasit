#!/usr/bin/env python3
"""
    Transform google-book-ngram format to phrasit's import format.

    This file is part of PhrasIt.

    PhrasIt is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PhrasIt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PhrasIt.  If not, see <http://www.gnu.org/licenses/>.
"""

import sys
from signal import signal, SIGPIPE, SIG_DFL
signal(SIGPIPE, SIG_DFL)

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
                if current_ngram != "":
                    print("\t".join([current_ngram, str(freq)]))
                freq = 0
                current_ngram = tmp[0]


if __name__ == "__main__":
    main()