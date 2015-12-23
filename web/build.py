#!/usr/bin/env python3
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