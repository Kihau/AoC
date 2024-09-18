#!/bin/sh

set -xe

mkdir -p build/
nasm -f elf64 src/main.asm    -i src/ -o build/main.o
nasm -f elf64 src/scratch.asm -i src/ -o build/scratch.o
nasm -f elf64 src/utils.asm   -i src/ -o build/utils.o

nasm -f elf64 src/days/day1.asm -i src/ -o build/day1.o
nasm -f elf64 src/days/day2.asm -i src/ -o build/day2.o
nasm -f elf64 src/days/day3.asm -i src/ -o build/day3.o
nasm -f elf64 src/days/day4.asm -i src/ -o build/day4.o

nasm -f elf64 src/os/linux.asm -i src/ -o build/linux.o

objects="build/main.o 
         build/scratch.o 
         build/utils.o 
         build/day1.o 
         build/day2.o 
         build/day3.o 
         build/day4.o
         build/linux.o"

ld -e main $objects -o build/aoc
./build/aoc
# strace ./aoc

