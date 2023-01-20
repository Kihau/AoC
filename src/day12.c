#include "all_days.h"

#define MAP_HEIGHT 41
#define MAP_WIDTH 101
// #define MAP_WIDTH 77
#define DISTANCE_INF -1
#define QUEUE_CAPACITY MAP_HEIGHT * MAP_WIDTH

#ifndef NCURSES
#define NCURSES true
#endif

typedef struct {
    int x;
    int y;
} Pos;

typedef struct {
    Pos data[QUEUE_CAPACITY];
    int start;
    int end;
} Queue;

static void print_map(
    const char map[MAP_HEIGHT][MAP_WIDTH], const int visit_map[MAP_HEIGHT][MAP_WIDTH],
    const Queue *queue
) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            move(y, x);
            if (visit_map[y][x] != DISTANCE_INF) {
                attron(COLOR_PAIR(1));
            }
            printw("%c", map[y][x]);
            attron(COLOR_PAIR(2));
        }
    }

    for (int i = queue->start; i < queue->end; i++) {
        Pos p = queue->data[i];
        move(p.y, p.x);
        attron(COLOR_PAIR(3));
        printw("%c", map[p.y][p.x]);
        attron(COLOR_PAIR(2));
    }
    // Pos last = queue->data[queue->end - 1];
    move(MAP_HEIGHT, 0);
}

static void print_visitmap(const int visit_map[MAP_HEIGHT][MAP_WIDTH]) {
    bool color_switch = true;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            color_switch ? attron(COLOR_PAIR(5)) : attron(COLOR_PAIR(6));
            color_switch = !color_switch;

            printw("%3i", visit_map[y][x]);
            attron(COLOR_PAIR(2));
        }
        printw("\n");
    }
}

#define TEST_PATH_BACKWARDS(pos_x, pos_y)                                                          \
    curr - 1 == visit_map[pos_y][pos_x] && map[p.y][p.x] - map[pos_y][pos_x] <= 1

#define TEST_PATH(pos_x, pos_y)                                                                    \
    curr - 1 == visit_map[pos_y][pos_x] && map[pos_y][pos_x] - map[p.y][p.x] <= 1

static void print_map_path(
    const char map[MAP_HEIGHT][MAP_WIDTH], const int visit_map[MAP_HEIGHT][MAP_WIDTH], Pos p, 
    bool draw_backwards
) {
    int curr = visit_map[p.y][p.x];
    while (curr > 0) {
        struct timespec b, a = { 0, 10000000 };
        nanosleep(&a, &b);
        move(p.y, p.x);
        attron(COLOR_PAIR(4));
        printw("%c", map[p.y][p.x]);
        attron(COLOR_PAIR(2));
        refresh();

        if (draw_backwards) {
            if (p.y - 1 >= 0 && TEST_PATH_BACKWARDS(p.x, p.y - 1)) {
                p.y--;
            } else if (p.y + 1 < MAP_HEIGHT && TEST_PATH_BACKWARDS(p.x, p.y + 1)) {
                p.y++;
            } else if (p.x - 1 >= 0 && TEST_PATH_BACKWARDS(p.x - 1, p.y)) {
                p.x--;
            } else if (p.x + 1 < MAP_WIDTH && TEST_PATH_BACKWARDS(p.x + 1, p.y)) {
                p.x++;
            }
        } else {
            if (p.y - 1 >= 0 && TEST_PATH(p.x, p.y - 1)) {
                p.y--;
            } else if (p.y + 1 < MAP_HEIGHT && TEST_PATH(p.x, p.y + 1)) {
                p.y++;
            } else if (p.x - 1 >= 0 && TEST_PATH(p.x - 1, p.y)) {
                p.x--;
            } else if (p.x + 1 < MAP_WIDTH && TEST_PATH(p.x + 1, p.y)) {
                p.x++;
            }
        } 

        curr--;
    }

    move(p.y, p.x);
    attron(COLOR_PAIR(4));
    printw("%c", map[p.y][p.x]);
    attron(COLOR_PAIR(2));
    refresh();

    move(MAP_HEIGHT, 0);
}

static void print_visitmap_path(
    const char map[MAP_HEIGHT][MAP_WIDTH], const int visit_map[MAP_HEIGHT][MAP_WIDTH], Pos p, 
    bool draw_backwards
) {
    int curr = visit_map[p.y][p.x];

    while (curr > 0) {

        Pos offset = { .x = p.x * 3, .y = p.y + MAP_HEIGHT };
        move(offset.y, offset.x);

        attron(COLOR_PAIR(8));
        printw("%3i", visit_map[p.y][p.x]);
        attron(COLOR_PAIR(2));

        if (draw_backwards) {
            if (p.y - 1 >= 0 && TEST_PATH_BACKWARDS(p.x, p.y - 1)) {
                p.y--;
            } else if (p.y + 1 < MAP_HEIGHT && TEST_PATH_BACKWARDS(p.x, p.y + 1)) {
                p.y++;
            } else if (p.x - 1 >= 0 && TEST_PATH_BACKWARDS(p.x - 1, p.y)) {
                p.x--;
            } else if (p.x + 1 < MAP_WIDTH && TEST_PATH_BACKWARDS(p.x + 1, p.y)) {
                p.x++;
            }
        } else {
            if (p.y - 1 >= 0 && TEST_PATH(p.x, p.y - 1)) {
                p.y--;
            } else if (p.y + 1 < MAP_HEIGHT && TEST_PATH(p.x, p.y + 1)) {
                p.y++;
            } else if (p.x - 1 >= 0 && TEST_PATH(p.x - 1, p.y)) {
                p.x--;
            } else if (p.x + 1 < MAP_WIDTH && TEST_PATH(p.x + 1, p.y)) {
                p.x++;
            }
        } 

        curr--;
    }

    Pos offset = { .x = p.x * 3, .y = p.y + MAP_HEIGHT };
    move(offset.y, offset.x);
    attron(COLOR_PAIR(8));
    printw("%3i", visit_map[p.y][p.x]);
    attron(COLOR_PAIR(2));

    move(MAP_HEIGHT, 0);
}

// TODO(???): Also when a position was visited but the path can be shorter - update it
#define TEST_AND_MODIFY_POS(pos)                                                                   \
    if (visit_map[pos.y][pos.x] == DISTANCE_INF && map[pos.y][pos.x] - map[cur.y][cur.x] <= 1      \
        /* HACK: Simplification based on the input. Remove check below if the output is wrong */   \
        && !(map[pos.y][pos.x] == 'a' && map[cur.y][cur.x] == 'c')) {                              \
        visit_map[pos.y][pos.x] = visit_map[cur.y][cur.x] + 1;                                     \
        queue.data[queue.end++] = pos;                                                             \
    }

static int find_shortest_path(
    const char map[MAP_HEIGHT][MAP_WIDTH], int visit_map[MAP_HEIGHT][MAP_WIDTH],
    const Pos start, const Pos end
) {
    visit_map[start.y][start.x] = 0;

    Queue queue = {};
    queue.data[queue.end++] = start;

    while (queue.start != queue.end && queue.end != QUEUE_CAPACITY) {
        // Dequeue first element.
        Pos cur = queue.data[queue.start++];

        // Check if the destination was reached.
        if (cur.x == end.x && cur.y == end.y)
            break;

        // Mark adjacent elements of current as visited and add them to the queue.
        Pos up = { cur.x, cur.y - 1 };
        if (up.y >= 0) {
            TEST_AND_MODIFY_POS(up);
        }

        Pos right = { cur.x + 1, cur.y };
        if (right.x < MAP_WIDTH) {
            TEST_AND_MODIFY_POS(right);
        }

        Pos down = { cur.x, cur.y + 1 };
        if (down.y < MAP_HEIGHT) {
            TEST_AND_MODIFY_POS(down);
        }

        Pos left = { cur.x - 1, cur.y };
        if (left.x >= 0) {
            TEST_AND_MODIFY_POS(left);
        }

    #if NCURSES
        struct timespec b, a = { 0, 100000 };
        nanosleep(&a, &b);

        print_map(map, visit_map, &queue);
        print_visitmap(visit_map);
        refresh();
    #endif
    }

#if NCURSES
    print_map_path(map, visit_map, end, true);
    print_visitmap_path(map, visit_map, end, true);
    refresh();
    if (visit_map[end.y][end.x] != DISTANCE_INF) {
        getchar();
    }
#endif

    return visit_map[end.y][end.x];
}

#define TEST_AND_MODIFY_POS_BACKWARDS(pos)                                                         \
    if (visit_map[pos.y][pos.x] == DISTANCE_INF && map[cur.y][cur.x] - map[pos.y][pos.x] <= 1      \
        /* HACK: Simplification based on the input. Remove check below if the output is wrong */   \
        /* && !(map[pos.y][pos.x] == 'a' && map[cur.y][cur.x] == 'c') */) {                        \
        visit_map[pos.y][pos.x] = visit_map[cur.y][cur.x] + 1;                                     \
        queue.data[queue.end++] = pos;                                                             \
    }

static int find_closest_reachable_a(
    const char map[MAP_HEIGHT][MAP_WIDTH], int visit_map[MAP_HEIGHT][MAP_WIDTH], const Pos start 
) {
    visit_map[start.y][start.x] = 0;

    Queue queue = { 
        .start = 0,
        .end = 0,
    };
    queue.data[queue.end++] = start;

    Pos end = { -1, -1 };
    while (queue.start != queue.end && queue.end != QUEUE_CAPACITY) {
        // Dequeue first element.
        Pos cur = queue.data[queue.start++];

        if (map[cur.y][cur.x] == 'a') {
            end.x = cur.x; 
            end.y =  cur.y;
            break;
        }

        // Mark adjacent elements of current as visited and add them to the queue.
        Pos up = { cur.x, cur.y - 1 };
        if (up.y >= 0) {
            TEST_AND_MODIFY_POS_BACKWARDS(up);
        }

        Pos right = { cur.x + 1, cur.y };
        if (right.x < MAP_WIDTH) {
            TEST_AND_MODIFY_POS_BACKWARDS(right);
        }

        Pos down = { cur.x, cur.y + 1 };
        if (down.y < MAP_HEIGHT) {
            TEST_AND_MODIFY_POS_BACKWARDS(down);
        }

        Pos left = { cur.x - 1, cur.y };
        if (left.x >= 0) {
            TEST_AND_MODIFY_POS_BACKWARDS(left);
        }

    #if NCURSES
        struct timespec b, a = { 0, 100000 };
        nanosleep(&a, &b);

        print_map(map, visit_map, &queue);
        print_visitmap(visit_map);
        refresh();
    #endif
    }

#if NCURSES
    print_map_path(map, visit_map, end, false);
    print_visitmap_path(map, visit_map, end, false);
    refresh();
    if (visit_map[end.y][end.x] != DISTANCE_INF) {
        getchar();
    }
#endif

    return visit_map[end.y][end.x];
}

void init_ncurses() {
    initscr();
	noecho();
    start_color();

    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_BLUE);

    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_YELLOW, COLOR_BLACK);
    init_pair(8, COLOR_RED, COLOR_BLACK);
}

void day12(FILE *input) {
#if NCURSES
    init_ncurses();
#endif

    // TODO: This is the input scan (it kind of sucks ya'know)
    //       Map could be a structure or something.
    // char buffer[256];
    // int width = 0, height = 0;
    // fgets(buffer, sizeof(buffer), input);
    // while (buffer[width])
    //     width++;
    //
    // while (fgets(buffer, sizeof(buffer), input))
    //     height++;
    // fseek(input, 0, SEEK_SET);
    //
    // // Horrible heap allocations
    // char **map = malloc(height);
    // for (int i = 0; i < height; i++)
    //     map[i] = malloc(width);
    //
    // // Deallocate heap stuff
    // for (int i = 0; i < height; i++)
    //     free(map[i]);
    // free(map);

    // TODO: Scan the input - find its width and height so the hardcoded stuff incident doesn't
    // happened anymore. Then allocate it on the head, one big allocation shouldn't be a problem,
    // right? (RIGHT?!)
    // Is possible to just know the line count, or??
    char map[MAP_HEIGHT][MAP_WIDTH] = {};
    for (int i = 0; i < MAP_WIDTH; i++) {
        fread(map[i], 1, MAP_WIDTH, input);
        fseek(input, 1, SEEK_CUR);
    }

    Pos start = { -1, -1 };
    Pos end = { -1, -1 };

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (start.x != -1 && end.x != -1) {
                goto breaK_outer;
            }
            
            if (map[y][x] == 'S') {
                start.x = x;
                start.y = y;
            } else if (map[y][x] == 'E') {
                end.x = x;
                end.y = y;
            }
        }
    }
breaK_outer:
    while (0);

    map[start.y][start.x] = 'a';
    map[end.y][end.x] = 'z';

    int visit_map[MAP_HEIGHT][MAP_WIDTH];
    memset(visit_map, DISTANCE_INF, sizeof(visit_map));
    int part1 = find_shortest_path(map, visit_map, start, end);

    memset(visit_map, DISTANCE_INF, sizeof(visit_map));
    int part2 = find_closest_reachable_a(map, visit_map, end);

#if NCURSES
    endwin();
#endif

    printf("PART 1: Shortest path from S to E = %i\n", part1);
    printf("PART 2: Path length from closest a = %i\n", part2);
}
