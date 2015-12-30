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
