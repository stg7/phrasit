* use a json lib: e.g. https://github.com/nlohmann/json
* it is possible to replace boost gzip stuff with: https://github.com/mateidavid/zstr
* run web query processing in seperate thread
* think of storing ngram->id id->ngram things
    ngram -> id could be: hash(ngram) -> id, with using id -> ngram a == check can be performed
    * for hash use: haesni or something else


* faster import:
    * 1 thread reads from stdin and collect lines in vector, if "full"
    * another thread imports to phrasit

* python3 binding
* add mechanism that multiple instances of one storage folder can be started
    or throw a message that is not "seg fault"!
* performance improvements:
    * use multiple inverted index instances for faster answering queries,
        for each pos_and_n one inverted index
    * do some index compression (inverted index post lists can be compressed easyly)
    * build an additional header file for pos_and_n start positions for each key
    * pos_and_n= 10*pos + n  values are:
        {1,..,5}{1,..,5} therefore 25 combinations
    * inverted index compression and post list skipping, or a better intersection operation

* query processing after operation expansion can be done parallel
* improve operators, so that nesting [ [] ] or { {} } will work or will be disallowed


DONE
----
* solve seq fault, if n gram is not stored
    * import 1+2+3 sample and run a * query -> seq fault

* add compactation call for optimizing leveldb:
    * https://github.com/google/leveldb/blob/master/include/leveldb/db.h
        db->CompactRange(NULL, NULL);
    * done for all large leveldb instances (freqs, ids)

* performance: a *
    needed time: 162067 ms
    (bottleneck: sorting)

    with first sort all freqs as tuple:
        140556 ms   238s
    with quickselect:
        247805 ms -> bad
            135 s -> now phrasit uses caching

* replace boost::filesystem with stl filesystem
* just store n_gram_and_pos in seperate file instead of:
    * store start pos for each n_gram_and_pos in separate file,
        -> n_gram_and_pos must not be stored

* use gzip files for external sort and _tmp, _sorted
    * [INFO ] sort_test.cpp@117      :  needed time: 37598.8  external sort, without gzip
    * [INFO ] sort_test.cpp@128      :  needed time: 99727.5 with gzip fast
* build unittests based on "test.cpp" skeleton
* add Licence GPLv3 to all files
* SEG Fault importing just a few ngrams: missing close call for _tmp file
* query log file: per default: storagedir/_query_log
* dataset generator, e.g. using pdfs as input to generate ngram-> freq files (see ngram-extractor)
