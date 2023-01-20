OUTPUT := aoc
SRC := main.c day1.c day2.c day3.c day4.c day5.c day6.c day7.c day8.c day9.c day10.c day11.c \
	   day12.c day13.c day14.c day15.c day16.c day17.c day18.c

CC := clang
CFLAGS := -g -Wall -Wextra -Wfloat-equal -fstack-protector
DFLAGS := -DNCURSES=true -DDUMMY=true
LDFLAGS := -lm -lncurses

all: 
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) -o $(OUTPUT)

install:
	@echo "Wait what? Whould would you install something like this???"
