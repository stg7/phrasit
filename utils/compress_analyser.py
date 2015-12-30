#!/usr/bin/env python3
"""
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

import os
import sys
import string

def escape(c):
    if len(c) != 1:
        return c
    if c in string.printable:
        return c
    c = ord(c)
    if c <= 0xff:
        return r'\\x{0:02x}'.format(c)
    elif c <= '\uffff':
        return r'\\u{0:04x}'.format(c)
    else:
        return r'\\U{0:08x}'.format(c)

def to_cpp(d):
    res = "std::string _comp_table[" + str(len(d.keys())) + "] = {"
    res += ",".join(["\"" + escape(d[x]) + "\"" for x in sorted(d.keys())])
    res += "};"
    return res


def main(params):

    histo = {}

    for l in sys.stdin:
        l = l[0:-1]
        tmp = l.split("\t")[0]
        for n in range(2, 5):
            for i in range(0, n * (len(tmp) // n), n):
                ngram = tmp[i:i + n]
                if " " not in ngram:
                    histo[ngram] = 1 + histo.get(ngram, 0)

    top = sorted(histo.items(), key=lambda x:x[1] * len(x[0]), reverse=True)[0:31]

    # from 0..31 non printable chars, 0 is reserverd for string end \0
    # therefore 1..31 can be used as compressing symbols

    comp_table = {0:r"\0"}

    j = 1
    for k, v in sorted(top, key=lambda x: len(x[0]), reverse=True):
        comp_table[j] = k
        j += 1

    """for i in range(256):
        if i not in comp_table:
            comp_table [i] = chr(i)
    """
    print("d = " + str(comp_table))

    print()
    print(to_cpp(comp_table))


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))