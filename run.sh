#!/bin/sh

# Builds and runs the code
if [ "$2" == "" ]; then
    RUNOPTS=$1
else
    MAKEOPTS=$1
    if [ "$2" != "all" ]; then
        RUNOPTS=$2
    fi
fi

bear -- make $MAKEOPTS 
cd build/
./aoc $RUNOPTS
