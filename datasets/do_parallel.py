#!/usr/bin/env python3
"""
tool for running scripts parallel
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
    parser = argparse.ArgumentParser(description='run a command on several files parallel', epilog="stg7 2015", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--cpu_count',type=int, default=multiprocessing.cpu_count(), help='thread/cpu count')
    parser.add_argument('--script', type=str, default="echo ", help='script for handling one file')
    parser.add_argument('infile',nargs="+", type=str, help='inputfile')
    argsdict = vars(parser.parse_args())

    cpu_count = argsdict["cpu_count"]
    lInfo("running with " + str(cpu_count) + " threads")

    script = argsdict["script"]
    files = zip(argsdict["infile"], [script for x in argsdict["infile"]])
    global files_count
    files_count = len(argsdict["infile"])
    pool = Pool(processes=cpu_count)

    params = files
    pool.map(do_it, params)

    lInfo("done.")


if __name__ == "__main__":
    main(sys.argv[1:])