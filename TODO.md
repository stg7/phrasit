* performance improvements:
    * use multiple inverted index instances for faster answering queries,
        for each pos_and_n one inverted index
    * do some index compression (inverted index post lists can be compressed easyly)
    * build an additional header file for pos_and_n start positions for each key
    * pos_and_n= 10*pos + n  values are:
        {1,..,5}{1,..,5} therefore 25 combinations

* dataset generator, e.g. using pdfs as input to generate ngram-> freq files
* think of storing ngram->id id->ngram things
* query processing after operation expansion can be done parallel
* inverted index compression and post list skipping, or a better intersection operation
* improve operators, so that nesting [ [] ] or { {} } will work or will be disallowed


DONE
----

* SEG Fault importing just a few ngrams: missing close call for _tmp file
* query log file: per default: storagedir/_query_log
