
OUTPUT := aoc
# SRC := src/main.c src/day1.c src/day2.c src/day3.c src/day4.c src/day5.c src/day6.c src/day7.c \
# 	   src/day8.c src/day9.c src/day10.c src/day11.c src/day12.c src/day13.c src/day14.c \
# 	   src/day15.c src/day16.c src/day17.c src/day18.c

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, build/%.o, $(SRC))

INCLUDE := "-Iinclude/"

CC := clang
CFLAGS := -g -Wall -Wextra -Wfloat-equal -fstack-protector
DFLAGS := -DNCURSES=true -DDUMMY=true
LDFLAGS := -lm -lncurses

all: build

build: $(OBJ)
	cp -r inputs/ build/
	$(CC) $^ $(CFLAGS) $(INCLUDE) $(LDFLAGS) $(DFLAGS) -o build/$(OUTPUT)

build/%.o: src/%.c
	$(CC) $(INCLUDE) $(CFLAGS) $(DFLAGS) -c -o $@ $<

clean:
	rm -rf build/$(OUTPUT)
	rm -rf build/*.o
	rm -rf build/inputs

install:
	@echo "Wait what? Whould would you install something like this???"


