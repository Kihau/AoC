
OUTPUT := build/aoc
SRC := src/main.c src/day1.c src/day2.c src/day3.c src/day4.c src/day5.c src/day6.c src/day7.c \
	   src/day8.c src/day9.c src/day10.c src/day11.c src/day12.c src/day13.c src/day14.c \
	   src/day15.c src/day16.c src/day17.c src/day18.c
INCLUDE := "-Iinclude/"

CC := clang
CFLAGS := -g -Wall -Wextra -Wfloat-equal -fstack-protector
DFLAGS := -DNCURSES=true -DDUMMY=true
LDFLAGS := -lm -lncurses

all: 
	cp -r inputs/ build/
	$(CC) $(INCLUDE) $(SRC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) -o $(OUTPUT)

install:
	@echo "Wait what? Whould would you install something like this???"
