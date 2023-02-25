#!/bin/sh

if [ "$1" == "clean" ]; then
    bear -- make clean
    shift
fi

# Builds and runs the code
if [ "$1" != "" ]; then
    if [ "$1" != "-" ]; then
        MAKEOPTS=$1
    fi
    bear -- make $MAKEOPTS
fi 

if [ "$2" != "" ]; then
    if [ "$2" != "-" ]; then
        RUNOPTS=$2
    fi
    cd build/
    ./aoc $RUNOPTS
fi
