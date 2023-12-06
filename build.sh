#!/bin/sh

set -xe

nasm -f elf64 day1.asm
ld day1.o -o day1
rm day1.o
./day1
# strace ./day1
