#include "all_days.h"

typedef struct {
    char dir;
    int len;
} Move;

typedef struct {
    int x;
    int y;
} Pos;

#define KNOTS_COUNT 10

typedef struct {
    Pos knots[KNOTS_COUNT];
} Rope;

// #define SIZE        400
// #define CROP_LEFT   350
// #define CROP_TOP    350
// #define CROP_DOWN   350
// #define CROP_RIGHT  350

#if NCURSES
    #define SIZE         20
#else 
    #define SIZE         400
#endif

#define CROP_LEFT    00
#define CROP_TOP     00
#define CROP_DOWN    00
#define CROP_RIGHT   00

static void print_state(bool grid[SIZE * 2][SIZE * 2], Rope *rope) {
    for (int i = SIZE * 2 - CROP_DOWN - 1; i >= CROP_TOP; i--) {
        for (int j = CROP_LEFT; j < SIZE * 2 - CROP_RIGHT; j++) {
            if (i == SIZE && j == SIZE) {
                printw("s");
                continue;
            } 

            for (int k = 0; k < KNOTS_COUNT; k++) {
                Pos knot = rope->knots[k];
                if (knot.y == i && knot.x == j) {
                    printw("%i", k);
                    goto continue_outer;
                }
            }

            if (grid[i][j]) printw("#");
            else printw(".");

        continue_outer:
            while(0);
        }

        printw("\n");
    }
    printw("\n");
}

static Rope crate_rope() {
    Rope rope;
    for (int i = 0; i < KNOTS_COUNT; i++) {
        Pos pos = { SIZE, SIZE };
        rope.knots[i] = pos;
    }
    return rope;
}

static void move_head(Rope *rope, Move move) {
    switch (move.dir) {
        case 'U': {
            rope->knots[0].y += 1;
        } break;
        case 'R': { 
            rope->knots[0].x += 1;
        } break;
        case 'D': {
            rope->knots[0].y -= 1;
        } break;
        case 'L': {
            rope->knots[0].x -= 1;
        } break;
        default: assert("Invalid direction. Is the input correct?"); break;
    }
}

static void move_rope(Rope *rope, Move move) {
    move_head(rope, move);

    for (int i = 0; i < KNOTS_COUNT - 1; i++) {
        // Sub-head knot
        Pos *k1 = rope->knots + i;
        // Sub-tail knot
        Pos *k2 = rope->knots + i + 1;

        int x_dist = k1->x - k2->x;
        int y_dist = k1->y - k2->y;

        // The distance between knots on either of axis is greater than 2 so the knot k2 
        // must move closer to the knot k1.
        if (abs(x_dist) > 1 || abs(y_dist) > 1) {
            // Here position of knots are different on both axis
            if (k1->x != k2->x && k1->y != k2->y) {
                // Move diagonally towards the knot k1.
                // Either position x and y needs to be changed for the knot k2.
                if (x_dist > 0)
                    k2->x += 1;
                else k2->x -= 1;

                if (y_dist > 0)
                    k2->y += 1;
                else k2->y -= 1;

            // Either x or y axis is alligned. Only x or y has to be changed
            } else {
                if (k1->x != k2->x && k1->y == k2->y) {
                    // Move on the x axis
                    if (x_dist > 0)
                        k2->x += 1;
                    else k2->x -= 1;
                } else {
                    // Move on the y axis
                    if (y_dist > 0)
                        k2->y += 1;
                    else k2->y -= 1;
                }
            }
        }
    }
}

static void from_console_input(
    Rope *rope, bool part1[SIZE * 2][SIZE * 2], bool part2[SIZE * 2][SIZE * 2]
) {
    print_state(part2, rope);
    char c = getchar();
    while (c) {
        switch (c) {
            case 'w': c = 'U'; break;
            case 's': c = 'D'; break;
            case 'a': c = 'L'; break;
            case 'd': c = 'R'; break;
            case 'q': case 'e': return;
        }
        Move m = { c, 1 };
        move_rope(rope, m);
        Pos t1 = rope->knots[1];
        part1[t1.y][t1.x] = true;
        Pos t2 = rope->knots[KNOTS_COUNT - 1];
        part2[t2.y][t2.x] = true;

        // printf("%c %i %i\n", m.dir, m.len, i);
        clear();
        print_state(part2, rope);
        refresh();
        c = getchar();
    }
}

static void from_input_file(
    FILE *input, Rope *rope, bool part1[SIZE * 2][SIZE * 2], bool part2[SIZE * 2][SIZE * 2]
) {
    Move m;
    char buffer[8];
    while (fgets(buffer, sizeof(buffer), input)) {
        m.dir = buffer[0];
        m.len = atoi(buffer + 2);

        for (int i = 0; i < m.len; i++) {
            move_rope(rope, m);
            Pos t1 = rope->knots[1];
            part1[t1.y][t1.x] = true;
            Pos t2 = rope->knots[KNOTS_COUNT - 1];
            part2[t2.y][t2.x] = true;
        }
    }
}

void day9(FILE *input) { 
    // TODO: PERF: 
    //     This MEGA SUCKS. Should I create hash set instead or
    //     operate on bitsets (or something else)?
    bool part1[SIZE * 2][SIZE * 2] = {}; 
    bool part2[SIZE * 2][SIZE * 2] = {}; 

    Rope rope = crate_rope();

#if NCURSES
	initscr();
	noecho();
    clear();

    from_console_input(&rope, part1, part2);
    clear();
    endwin();
#else
    from_input_file(input, &rope, part1, part2);
#endif

    int part1_moves = 0;
    int part2_moves = 0;
    for (int i = 0; i < SIZE * 2; i++) {
        for (int j = 0; j < SIZE * 2; j++) {
            if (part1[i][j]) part1_moves++;
            if (part2[i][j]) part2_moves++;
        }
    }
    printf("PART1: 2-knot tail moves = %i\n", part1_moves);
    printf("PART2: 10-knot tail moves = %i\n", part2_moves);
}
