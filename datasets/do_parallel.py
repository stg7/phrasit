#!/usr/bin/env python3
"""
    tool for running scripts parallel

    example usage:
        ./do_parallel.py *.gz --script ./whatever.sh -s

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
import sys
import os
import argparse
from multiprocessing import Pool
import multiprocessing

files_done = 0
files_count = 0

def lInfo(msg):
    print("[Info] " + str(msg))

def do_it(params):
    infile = params[0]
    script = params[1]
    res = os.system(script + " " + infile)
    if res != 0:
        sys.exit(1)
    global files_done
    files_done += 1
    lInfo("done {}".format(infile))
    lInfo("{}/{}".format(files_done, files_count))

def main(params):
    parser = argparse.ArgumentParser(description='run a command on several files parallel', epilog="stg7 2016", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--cpu_count',type=int, default=multiprocessing.cpu_count(), help='thread/cpu count')
    parser.add_argument('--script', type=str, default="echo ", help='script for handling one file')
    parser.add_argument('-s', dest='sortbysize', action='store_true', help='sort by size starting with smallest file')
    parser.add_argument('infile',nargs="+", type=str, help='inputfile')
    argsdict = vars(parser.parse_args())

    cpu_count = argsdict["cpu_count"]
    lInfo("running with " + str(cpu_count) + " threads")

    script = argsdict["script"]
    if argsdict["sortbysize"]:
        argsdict["infile"].sort(key=lambda x: os.path.getsize(x))

    resorted_files = []
    for i in range(cpu_count):
        resorted_files += [argsdict["infile"][x] for x in range(0, len(argsdict["infile"])) if x % cpu_count == i]


    files = zip(resorted_files, [script for x in resorted_files])

    global files_count
    files_count = len(argsdict["infile"])
    pool = Pool(processes=cpu_count)

    params = files
    pool.map(do_it, params)

    lInfo("done.")


if __name__ == "__main__":
    main(sys.argv[1:])