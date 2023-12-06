#!/bin/sh

set -xe

nasm -f elf64 src/main.asm  -i src/ -o build/main.o
nasm -f elf64 src/utils.asm -i src/ -o build/utils.o
nasm -f elf64 src/day1.asm  -i src/ -o build/day1.o

ld  build/main.o build/utils.o build/day1.o -o aoc
./aoc
# strace ./aoc
