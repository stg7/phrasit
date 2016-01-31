#!/usr/bin/env python3
"""
    printout files that are not recompressed complete

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
import os
import argparse

def main(params):
    parser = argparse.ArgumentParser(description='TODO', epilog="stg7 2016", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('infolder', type=str, help='input folder')

    argsdict = vars(parser.parse_args())

    infolder = argsdict['infolder']
    lzma = set([infolder + "/" + x.replace(".lzma", "") for x in os.listdir(infolder) if ".lzma" in x])
    gz = set([infolder + "/" + x.replace(".gz", "") for x in os.listdir(infolder) if ".gz" in x])
    for i in list(lzma & gz):
        print(i + ".lzma")

if __name__ == "__main__":
    main(sys.argv[1:])