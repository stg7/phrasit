#!/usr/bin/env python3

import random

def gb(x):
    return x * 1024 * 1024 * 1024

size = gb(10)

created_bytes = 0

i = 0
key = 0
while created_bytes < size:
    n = random.randint(1, 5)
    ni = i
    line = "{}\t{}\t{}".format(key, ni, n) + "\n"
    print(line, end="")
    i += 1
    if i % 1000:
        key += 1
    created_bytes += len(line)
