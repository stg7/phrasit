# -*- coding: utf8 -*-
"""
    part of phrasit

    author: Steve Göring
    contact: stg7@gmx.de
    2015
"""
import os
import sys
import glob
import multiprocessing

SetOption('num_jobs', multiprocessing.cpu_count()) # build with all aviable cpu cores/threads

stylechecker = Builder(action='./test_convention.sh $SOURCES', suffix='', src_suffix='')
valgrind = Builder(action='valgrind ./$SOURCES', suffix='', src_suffix='')

os.system("./prepare.sh")

env = Environment(CPPPATH = ["src/", "libs/"],
                  BUILDERS = {'StyleCheck' : stylechecker, 'Valgrind': valgrind})

# include local installed libs
libspath = 'libs/'
libs = [] #['tbb']
env.Append(
    LIBPATH=['.'] + [libspath + x +  "/build/lib" for x in libs],
    CPPPATH=[libspath + x + "/build/include" for x in libs]
)

# leveldb
env.Append(
    CPPPATH = [libspath + "leveldb/include/"],
    LIBPATH = [libspath + 'leveldb/']
)
env.Append(LINKFLAGS=['-Wl,--rpath,' + libspath + 'leveldb/', '-pthread'])

# mongoose-cpp
env.Append(
    CPPPATH = [libspath + "mongoose-cpp/"],
    LIBPATH = [libspath + "mongoose-cpp/"]
)

env.Decider('MD5')

conf = Configure(env)

needed_libs = ['leveldb', 'mongoose', 'boost_unit_test_framework', 'boost_program_options', 'boost_system', 'boost_filesystem'] # ,'tbb'
for lib in needed_libs:
    if not conf.CheckLib(lib, language="c++"):
        print "Unable to find lib: " + lib + ". Exiting."
        exit(-1)

needed_headers = ['boost/program_options.hpp', 'boost/filesystem.hpp', 'boost/test/unit_test.hpp'] #'tbb/tbb.h',
for header in needed_headers:
    if not conf.CheckCXXHeader(header):
        print "Unable to find header: " + header + ". Exiting."
        sys.exit(-1)


env.Append(CXXFLAGS=['-std=c++11'])

# if you call scons debug=1 debug build is activated
if ARGUMENTS.get('debug', 0) != 0:
    # more checks
    env.Append(CXXFLAGS=['-Wall','-pedantic-errors', '-g'])
    # "no" optimization
    env.Append(CXXFLAGS=['-O0'])
else:
    env.Append(CXXFLAGS=['-march=native']) # use native architecture
    #env.Append(CXXFLAGS=['-mavx'])
    env.Append(CXXFLAGS=['-Wall'])
    env.Append(CXXFLAGS=['-O3'])

    # loop unrolling and link time optimization, options should be tested
    env.Append(CXXFLAGS=['-funroll-loops', '-flto', '-fwhole-program'])

#env.Append(LINKFLAGS=['-Wl,--rpath,./libs/tbb/build/lib/'])

testcases = set(glob.glob("src/tests/*.cpp"))

header = set(glob.glob("src/*.hpp") +  glob.glob("src/*/*.hpp"))
sources = set(glob.glob("src/*.cpp") + glob.glob("src/*/*.cpp")) - set(["src/main.cpp", "src/tests/unittests.cpp"]) - testcases

libs = glob.glob("libs/*/*.c")

# check code conventions and build programm
#env.StyleCheck("conventions", ["src/main.cpp"] + list(sources) + list(header) + list(testcases))

# build unittests
#unittests = env.Program('unittest', ["src/tests/unittests.cpp"] + list(sources) + libs)
#test_alias = Alias('test', [unittests], unittests[0].path)
#AlwaysBuild(test_alias)


env.Program('phrasit', ["src/main.cpp"] + list(sources) + libs)

#env.ParseConfig('/usr/bin/python3-config --includes --libs')
#env.Append(LIBS=['boost_python3'])

#env.SharedLibrary('positronic.so', ["src/py/positronic.cpp"] + list(sources) + libs, SHLIBPREFIX='')
#env.Valgrind("valgrind" ,'positronic')
