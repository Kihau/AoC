#ifndef ALL_DAYS_H
#define ALL_DAYS_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <signal.h>

// Rdtsc
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#ifndef DUMMY
    #define DUMMY false
#endif

#ifndef NCURSES
    #define NCURSES true
#endif

#if NCURSES
    #include <curses.h>
#else
    #define printw printf
#endif

typedef char i8;
typedef short i16;
typedef int i32;
typedef long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

// TODO: Custom assert that exits to main?

void day1(FILE *input);
void day2(FILE *input);
void day3(FILE *input);
void day4(FILE *input);
void day5(FILE *input);
void day6(FILE *input);
void day7(FILE *input);
void day8(FILE *input);
void day9(FILE *input);
void day10(FILE *input);

void day11(FILE *input);
void day12(FILE *input);
void day13(FILE *input);
void day14(FILE *input);
void day15(FILE *input);
void day16(FILE *input);
void day17(FILE *input);
void day18(FILE *input);
void day19(FILE *input);
void day20(FILE *input);

void day21(FILE *input);
void day22(FILE *input);
void day23(FILE *input);
void day24(FILE *input);
void day25(FILE *input);

#endif // ALL_DAYS_H
