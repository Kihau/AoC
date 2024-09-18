#!/bin/sh

set -xe

mkdir -p build/
nasm -f win64 src/main.asm     -i src/ -o build/main.obj
nasm -f win64 src/scratch.asm  -i src/ -o build/scratch.obj
nasm -f win64 src/utils.asm    -i src/ -o build/utils.obj

nasm -f win64 src/days/day1.asm -i src/ -o build/day1.obj
nasm -f win64 src/days/day2.asm -i src/ -o build/day2.obj
nasm -f win64 src/days/day3.asm -i src/ -o build/day3.obj
nasm -f win64 src/days/day4.asm -i src/ -o build/day4.obj

nasm -f win64 src/os/windows.asm -i src/ -o build/windows.obj

objects="build/main.obj 
         build/scratch.obj 
         build/utils.obj 
         build/day1.obj 
         build/day2.obj 
         build/day3.obj 
         build/day4.obj
         build/windows.obj"

clang -g -O0 -target x86_64-pc-win32-gnu $objects -o build/aoc.exe
# wine ./build/aoc.exe

# TODO: Should be a part of separate script in build-windows.bat
# clang -target x86_64-pc-win32-msvc $objects -o build/aoc.exe
# ./build/aoc.exe
