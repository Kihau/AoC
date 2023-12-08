#!/bin/sh

set -xe

mkdir -p build/
nasm -f elf64 src/main.asm  -i src/ -o build/main.o
nasm -f elf64 src/utils.asm -i src/ -o build/utils.o
nasm -f elf64 src/day1.asm  -i src/ -o build/day1.o
nasm -f elf64 src/day2.asm  -i src/ -o build/day2.o

ld build/main.o build/utils.o build/day1.o build/day2.o -o build/aoc
./build/aoc
# strace ./aoc
