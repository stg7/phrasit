# netspeak reverse engineering and core concepts

author:
> steve göring

Netspeak is a closed source but public accessible tool to assist writers [\[0\]](http://www.netspeak.org/).

## API
The Netspeak API is documented and open accessible:

The following table defines the syntax of netspeak queries [\[1\]](http://www.netspeak.org/#developer):

| Operator  |     Name      | Example                   | Description     |
|-----------|---------------|---------------------------|-----------------|
| ?         |    QMARK      | waiting ? response        | Matches exactly one word. |
| *         | ASTERISK      | waiting * response        | Matches zero or more words. |
| [ ]       | OPTIONSET     | the same [ like as ]      | Finds matches using each word in turn, including the empty word. Nesting of operators is not allowed. |
| \{ \}       | ORDERSET      | \{ only for members \}  | Finds matches of each permutation of the enclosed words. Nesting of operators is not allowed. |
| \#        | DICTSET       | waiting for #response     | Finds matches using the following word and each of its synonyms in turn. |

## Operator realization
General ideas for implementing all operators:
Suppose operator ? is implemented, then * is no problem:
```
    query = "a" * "b"

    generate new candidate queries with ? up to 5 words:
        "a" "b"
        "a" ? "b"
        "a" ? ? "b"
        "a" ? ? ? "b"
    get results for each query and sort by frequency
```

[ ] can be implemented in the following way:
```
    query = "a" ["x" "y" "z" .. ] "b"

    build queries in a rond robin way start with empty word as filler:
        "a" "b"
        "a" "x" "b"
        "a" "y" "b"
        "a" "z" "b"
        ....

    get each result and sort by frequency
```

\{ \} implementation:
```
    query = "a" \{ "x" "y" "z" \} "b"
    build each permutation of "x" "y" "z" .. (maximum 5 words)
        "a" "x" "y" "z" "b"
        "a" "y" "x" "z" "b"
        "a" "y" "z" "x" "b"
        "a" "z" "y" "x" "b"
        "a" "z" "x" "y" "b"
        "a" "x" "z" "y" "b"
```

\# is using synonyms therefore a synonym dictionary is needed, suppose you have all synonyms, then this operator is implementable
```
    query = "a" \#"x" "b"
    S = synonyms of "x"

    build for each synonym s in S a query:
        "a" "s" "b"
```

As a consequence of these observations it is necessary to have two base operators:

* ?
* synonym

## Data Source
Netspeak uses a non public dataset (ngram corpus) based on english documents from the web (google ngrams) [\[2\]](http://www.uni-weimar.de/en/media/chairs/webis/research/selected-projects/netspeak/) with approx 4 billion ngrams from length 1 to 5 words. All ngrams together have an uncompressed size of approx 100GB.

Where can we get a suitable dataset for our netspeak-clone?
Google published a ngram viewer with time annotated ngrams based on several books [\[3\]](http://storage.googleapis.com/books/ngrams/books/datasetsv2.html)

As a example you can download [\[4\]](http://storage.googleapis.com/books/ngrams/books/googlebooks-eng-all-2gram-20120701-ad.gz). The stored file format is straight forward:
```
    ngram TAB year TAB match_count TAB volume_count NEWLINE
```

First of all, this implementation should only use the english dataset.



## Implementation Ideas
* only implement ? operator
* store ngrams in a key-value thing (e.g. leveldb, or similiar), to get ids for each ngram
* use ngrams as "documents" in an inverted index
    * ngram1 = "a" "b" "c" "d" "e"  is stored
    ```
        "a" -> ngram1
        "b" -> ngram1
        "c" -> ngram1
        "d" -> ngram1
        "e" -> ngram1
    ```
    term vocabulary are 1-grams therefore id's should be stored, terms are restricted
* get a query with ?
    * (1) look for each term which ngrams are covered -> store all ngrams ids in-memory
    * (2) filter out ngrams that are not suitable
    ```
        query = "a ? house"

        (1) ngrams("a") + ngrams("house")

        filter out ngrams with lenght != 3 and that have not "a" as first and "house" as third word

        build it in a way to handle multiple ? operators
    ```
