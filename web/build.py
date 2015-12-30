#!/usr/bin/env python3
"""
    tool for converting php files to static html version

    example usage:
        ./build.py
        creates html files

        or

        ./build.py clean
        tidy up all created files

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

def main(params):
    """ Converts php files to static html files, if you call ./build.py clean,
        all html files will be deleted
    """
    if len(params) > 0 and params[0] == "clean":
        return os.system("rm -rf *.html")

    main_files = ["index.php", "about.php", "results.php"]
    for f in main_files:
        html_filename = os.path.splitext(f)[0] + ".html"
        os.system("""php {} | sed "s|.php|.html|g" > {} """.format(f, html_filename))
        print(html_filename)

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))