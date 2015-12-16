PhrasIt - a netspeak open source clone
======================================

Build PhrasIt
-------------
You only need:

* scons
* g++/gcc a new version which supports c++11
* boost

installed.

Then you can start building phrasit with:
```
scons
```
Scons will download all needed thirdparty libs and tools (e.g. leveldb) and build it,
this process should run without errors, otherwise you should run `./prepare.sh` manually for error checking.


Test it with a simple help screen call:
```
./phrasit -h
------------------------------------------------
phraseit - an opensource netspeak clone

Steve Göring 2015
Parameter:
  -h [ --help ]           produce help message
  -d [ --storagedir ] arg storage directory, default='storage'
  -f [ --queryfile ] arg  handle queries stored in a file
  -i [ --import ]         import from stdin, format: ngram tab freq
  -s [ --server ]         start phrasit in server mode

```

Download all needed ngrams
--------------------------
For downloading of all google-books-ngrams (english) you must start:
```
cd datasets
./download.py
```
and wait a long long time (the complete corpora is about ???300GB,
with my 6k dsl connection the download needed a few months!, maybe better go to university
with high speed internet connection).

After the download was successfully, you can convert the google-ngrams to the import format of phrasit with:
```
cd datasets
./transform_all.sh
```

Now you should have the following files:
```
└── datasets
    ├── download.list
    ├── download.py
    ├── googlebooks-eng-all-1gram-20120701-0.gz
....
    ├── sample
    ├── transform_all.sh
    └── transformed
        ├── 1gram
        ├── 2gram
        ├── 3gram
        ├── 4gram
        └── 5gram
```

In `datasets/transformed/*` are all transformed ngrams, you can now easily import it with:
```
cat datasets/transformed/?gram | ./phrasit -i
```

Importing all ngrams will need a lot of time and disk-space, therefore drink a coffee.