#!/usr/bin/env python3
"""
    tool for transforming ngram corpus (OLD)
    better use ./do_parallel.py *.gz --script ./transform.sh -s


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

import re
import os
from multiprocessing import Pool
import multiprocessing
import gzip


def lInfo(msg):
    print("[Info] " + str(msg))

def transform(params):
    infile = params[0]
    outfile = params[1]
    res = os.system("zcat {} | ./../utils/transform.py  >> {}".format(infile, outfile))
    if res != 0:
        sys.exit(1)

def main():
    cpu_count = multiprocessing.cpu_count()
    lInfo("running with " + str(cpu_count) + " threads")

    files = list(filter(lambda x: True if ".gz" in x else False, os.listdir(".")))
    # build groups
    file_group = {}
    for f in files:
        xgram = re.match(".*-(.gram).*", f).group(1)
        file_group[xgram] = file_group.get(xgram, []) +[f]

    outdir = "transformed_/"

    pool = Pool(processes=cpu_count)

    for k in sorted(file_group.keys()):
        params = zip(file_group[k], [outdir + k + "." + str(x % cpu_count) for x in range(len(file_group[k]))])
        pool.map(transform, params)

    lInfo("done.")


if __name__ == "__main__":
    main()