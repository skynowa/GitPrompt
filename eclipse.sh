#!/usr/bin/env bash
#
# \file  eclipse_gcc.sh
# \brief build tests
#


# vars
# export CC="gcc"
# export CXX="g++"

DIR="../GitPrompt"
DIR_BUILD="../GitPrompt_eclipse"


# build
mkdir -p $DIR_BUILD
cd $DIR_BUILD
cmake -G"Eclipse CDT4 - Unix Makefiles" $DIR
# make -j4 VERBOSE=1