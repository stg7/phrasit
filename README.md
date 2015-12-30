PhrasIt - a netspeak open source clone
======================================

Build PhrasIt
-------------
You only need:

* scons
* g++/gcc a new version which supports c++11
* boost
* php/a webserver

installed.

Then you can start building phrasit with:
```
scons
```
Scons will download all needed thirdparty libs and tools (e.g. leveldb) and build them,
this process should run without errors, otherwise you should run `./prepare.sh` manually for error checking.

Now you can test it with a simple help screen call:
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
and wait a long long time (the complete corpora is about 2-3TB,
with my 6k dsl connection the download needed a few months!, maybe better go to university
with a high speed internet connection).

You can start and stop the download, or e.g. start downloading all 2grams with
```
./download.py 2gram
```

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
        ....
```

In `datasets/transformed/*` are all transformed ngrams, you can now easily import it with:
```
cat datasets/transformed/?gram | ./phrasit -i
```

Importing all ngrams will need a lot of time and disk-space, therefore drink a coffee.

Converting to lzma
------------------
If you want to save disk-space you can transform all gz files to lzma using
```
cd datasets
./do_parallel.py *.gz ./convert.sh
```
This process will also need a lot of time and will use all logical cpu cores.

Input Format
------------
If you don't want to use the google-book-ngram dataset or you have an own dataset
then you need to know which input format PhrasIt uses. The format is quite easy:
```
ngram TAB freq
```
Where n-gram is, e.g. "hello world", TAB is a tabulator and freq is the absolute counted frequency.

At the moment, the input format is not customizable, if your data is stored in another format
you should build a convert script, that e.g. throw all ngrams in the inport format out to stdout.
Then you can easily build a processing pipeline like:
```
./myConvertScript myCoolDataset | ./phrasit -i
```

Web Part
--------
If you want to run the web-service you should start PhrasIt via
```
./phrasit -s
```

And you need to copy the web directory to a http server with php.
If php is not available you can build a static html version using `build.py` in the `web` folder.

It is important that you update the configuration file `config.json`:
```
var config = {
    "server_url": ["http://localhost:8090/api/"]
};
```

`server_url` must be your phrasit server with the correct port, this url must be reachable via internet,
localhost will only work in a local development setup. You can add multiple servers in the array
as a kind of load balancing. A server will be choosen randomly.

The server can run on another host, because all request will be done using jsonp.
