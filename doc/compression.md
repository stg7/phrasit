Compression comparison
======================
Example `googlebooks-eng-all-3gram-20120701-ka`:

* uncompressed: 5.2 gb
* gz compressed: approx 0.7 gb
* lzma compressed `lzma -9`: approx 0.45 gb

save 250 mb, 35%


5 1-gram files:

* gz compressed: approx 590 mb
* lzma compressed: approx 390 mb

save 200 mb, 34%


Recompression Pipeline
----------------------
```
zcat googlebooks-eng-all-3gram-20120701-ka.gz | lzma -9 -c -T 0 --stdout > googlebooks-eng-all-3gram-20120701-ka.lzma
```
