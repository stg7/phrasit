#!/usr/bin/env python3
"""
tool for transforming ngram corpus
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

    outdir = "transformed/"

    pool = Pool(processes=cpu_count)

    for k in sorted(file_group.keys()):
        params = zip(file_group[k], [outdir + k + "." + str(x % cpu_count) for x in range(len(file_group[k]))])
        pool.map(transform, params)

    lInfo("done.")


if __name__ == "__main__":
    main()