#!/bin/sh

# Exiting if anything fails
set -e

cd build/
cmake ..
mv compile_commands.json ..
make -j 12

if [ "$1" != "" ]; then
    if [ "$1" != "-" ]; then
        RUNOPTS=$@
    fi

    ./AoC $RUNOPTS
fi

# if [ "$1" == "clean" ]; then
#     bear -- make clean
#     shift
# fi
#
# # Builds and runs the code
# if [ "$1" != "" ]; then
#     if [ "$1" != "-" ]; then
#         MAKEOPTS=$1
#     fi
#     bear -- make $MAKEOPTS
#     RES="$?"
# fi 
#
# if [ "$2" != "" ]; then
#     if [ "$2" != "-" ]; then
#         RUNOPTS=$2
#     fi
#
#     if [ "$RES" == 0 ]; then
#         cd build/
#         ./aoc $RUNOPTS
#     fi
# fi
