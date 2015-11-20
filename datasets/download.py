#!/usr/bin/env python3
"""
tool for downloading ngram corpus from google books parallel
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

def main():
    f = open("download.list", "r")
    links = []
    for l in f:
        l = l.replace("\n", "")
        if "href" in l:
            link = l.replace("<a href=\"", "")
            link = re.sub("\">.*</a>", "", link)
            links.append(link)
    f.close()

    for l in links:
        download(l)
    """
    cpu_count = 1 # multiprocessing.cpu_count()
    lInfo("running with " + str(cpu_count) + " threads")

    pool = Pool(processes=cpu_count)
    pool.map(download, links)
    """


if __name__ == "__main__":
    main()