#!/usr/bin/env python3
"""
    tool for downloading ngram corpus from google books parallel

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
import sys
from multiprocessing import Pool
import multiprocessing

def lInfo(msg):
    print("[Info] " + msg)

def download(link):
    res = os.system("wget -c " + link)
    if res != 0:
        sys.exit(1)

def main(args):
    """ args[0] is an optional filter for download links, e.g. 3gram """
    f = open("download.list", "r")
    links = []
    for l in f:
        l = l.replace("\n", "")
        if "href" in l:
            link = l.replace("<a href=\"", "")
            link = re.sub("\">.*</a>", "", link)
            if len(args) == 1:
                if args[0] in link:
                    links.append(link)
            else:
                links.append(link)

    f.close()

    finished_links = []
    if os.path.isfile("finished.list"):
        f = open("finished.list", "r")
        finished_links = [x.strip() for x in f.readlines()]
        f.close()


    i = 0
    missing_links = sorted(list(set(links) - set(finished_links)))
    for l in missing_links:
        lInfo("{}/{}.".format(i, len(missing_links)))
        download(l)
        f = open("finished.list", "a")
        f.write(l + "\n")
        f.close()

        lInfo("{}/{} done.".format(i, len(missing_links)))
        i += 1
    """
    cpu_count = 1 # multiprocessing.cpu_count()
    lInfo("running with " + str(cpu_count) + " threads")

    pool = Pool(processes=cpu_count)
    pool.map(download, links)
    """


if __name__ == "__main__":
    main(sys.argv[1:])