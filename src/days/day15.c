#include "all_days.h"

typedef struct {
    int x;
    int y;
} Pos;

typedef struct {
    Pos position;
    Pos closest_beacon;
} Sensor;

#define PARSE_COORD(start, end)                                                                    \
    do {                                                                                           \
        while (*buffer++ != start);                                                                \
        while (*buffer++ != end) {                                                                 \
            assert(num_len != num_cap && "num_buf out of bounds");                                 \
            num_buf[num_len++] = *buffer;                                                          \
        }                                                                                          \
        num_buf[num_len] = 0;                                                                      \
        num_len = 0;                                                                               \
    } while (0);

static Sensor parse_sensor(char *buffer) {
    Sensor sensor;
    const int num_cap = 16;
    char num_buf[num_cap];
    int num_len = 0;

    PARSE_COORD('x', ',');
    sensor.position.x = atoi(num_buf);

    PARSE_COORD('y', ':');
    sensor.position.y = atoi(num_buf);

    PARSE_COORD('x', ',');
    sensor.closest_beacon.x = atoi(num_buf);

    PARSE_COORD('y', '\0');
    sensor.closest_beacon.y = atoi(num_buf);

    return sensor;
}

static void draw_map(Sensor *sensors, int sensors_len) {
    Pos offset = { 10, 10 };
    Pos size = { 30, 30 };
    initscr();
    noecho();
    start_color();

    for (int i = -offset.y; i < size.y + offset.y; i++) {
        for (int j = -offset.y; j < size.x + offset.x; j++) {
            move(i + offset.y, j + offset.y);
            printw(".");
        }
    }

    for (int s = 0; s < sensors_len; s++) {
        move(sensors[s].position.y + offset.y, sensors[s].position.x + offset.x);
        printw("S");
        move(sensors[s].closest_beacon.y + offset.y, sensors[s].closest_beacon.x + offset.x);
        printw("B");
    }

    refresh();
    getchar();
    endwin();
}

static void draw_map_one_sensor(Sensor *sensors, int sensors_len, int index) {
    Pos offset = { 10, 10 };
    Pos size = { 30, 30 };

    initscr();
    noecho();

    for (int i = -offset.y; i < size.y + offset.y; i++) {
        for (int j = -offset.y; j < size.x + offset.x; j++) {
            move(i + offset.y, j + offset.y);
            printw(".");
        }
    }

    Sensor sensor = sensors[index];
    int dist_sx = abs(sensor.position.x - sensor.closest_beacon.x);
    int dist_sy = abs(sensor.position.y - sensor.closest_beacon.y);
    int dist_s = dist_sx + dist_sy + 1;

    for (int i = sensor.position.y - dist_s; i < sensor.position.y + dist_s; i++) {
        for (int j = sensor.position.x - dist_s; j < sensor.position.x + dist_s; j++) {
            int dist_x = abs(sensor.position.x - j);
            int dist_y = abs(sensor.position.y - i);
            int dist = dist_x + dist_y + 1;
            if (dist <= dist_s) {
                move(i + offset.y, j + offset.y);
                printw("#");
            }
        }
    }

    for (int s = 0; s < sensors_len; s++) {
        move(sensors[s].position.y + offset.y, sensors[s].position.x + offset.x);
        printw("S");
        move(sensors[s].closest_beacon.y + offset.y, sensors[s].closest_beacon.x + offset.x);
        printw("B");
    }

    refresh();
    getchar();
    endwin();
}


static void draw_map_with_occupation(Sensor *sensors, int sensors_len) {
    Pos offset = { 10, 10 };
    Pos size = { 30, 30 };
    initscr();
    noecho();
    start_color();

    for (int i = -offset.y; i < size.y + offset.y; i++) {
        for (int j = -offset.y; j < size.x + offset.x; j++) {
            move(i + offset.y, j + offset.y);
            printw(".");
        }
    }

    for (int s = 0; s < sensors_len; s++) {
        int dist_sx = abs(sensors[s].position.x - sensors[s].closest_beacon.x);
        int dist_sy = abs(sensors[s].position.y - sensors[s].closest_beacon.y);
        int dist_s = dist_sx + dist_sy + 1;

        for (int i = sensors[s].position.y - dist_s; i < sensors[s].position.y + dist_s; i++) {
            for (int j = sensors[s].position.x - dist_s; j < sensors[s].position.x + dist_s; j++) {
                int dist_x = abs(sensors[s].position.x - j);
                int dist_y = abs(sensors[s].position.y - i);
                int dist = dist_x + dist_y + 1;
                if (dist <= dist_s) {
                    move(i + offset.y, j + offset.y);
                    printw("#");
                }
            }
        }
    }

    for (int s = 0; s < sensors_len; s++) {
        move(sensors[s].position.y + offset.y, sensors[s].position.x + offset.x);
        printw("S");
        move(sensors[s].closest_beacon.y + offset.y, sensors[s].closest_beacon.x + offset.x);
        printw("B");
    }

    move(offset.y, offset.x);
    printw("0");

    refresh();
    getchar();
    endwin();
}

static void draw_map_with_current_check(Sensor *sensors, int sensors_len, Pos pos, int index) {
    Pos offset = { 10, 10 };
    Pos size = { 30, 30 };

    init_pair(1, COLOR_BLACK, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);

    initscr();
	noecho();
    start_color();

    for (int i = -offset.y; i < size.y + offset.y; i++) {
        for (int j = -offset.y; j < size.x + offset.x; j++) {
            move(i + offset.y, j + offset.y);
            printw(".");
        }
    }


    Sensor sensor = sensors[index];
    int dist_sx = abs(sensor.position.x - sensor.closest_beacon.x);
    int dist_sy = abs(sensor.position.y - sensor.closest_beacon.y);
    int dist_s = dist_sx + dist_sy + 1;

    for (int i = sensor.position.y - dist_s; i < sensor.position.y + dist_s; i++) {
        for (int j = sensor.position.x - dist_s; j < sensor.position.x + dist_s; j++) {
            int dist_x = abs(sensor.position.x - j);
            int dist_y = abs(sensor.position.y - i);
            int dist = dist_x + dist_y + 1;
            if (dist <= dist_s) {
                move(i + offset.y, j + offset.y);
                printw("#");
            }
        }
    }

    for (int s = 0; s < sensors_len; s++) {
        move(sensors[s].position.y + offset.y, sensors[s].position.x + offset.x);
        printw("S");
        move(sensors[s].closest_beacon.y + offset.y, sensors[s].closest_beacon.x + offset.x);
        printw("B");
    }

    move(offset.y, offset.x);
    printw("0");

    attron(COLOR_PAIR(1));
    move(pos.y + offset.y, pos.x + offset.x);
    printw(" ");
    attron(COLOR_PAIR(2));

    move(size.y + offset.y, size.x + offset.x);
    refresh();
    getchar();
    endwin();
}

static bool is_occupied(Sensor *sensors, int sensors_len, long *radiuses, Pos pos) {
    for (int s = 0; s < sensors_len; s++) {
        // Skip all sensors
        if (sensors[s].position.x == pos.x && sensors[s].position.y == pos.y)
            continue;

        // Skip all beacons
        if (sensors[s].closest_beacon.x == pos.x && sensors[s].closest_beacon.y == pos.y)
            continue;

        long dist_x = abs(sensors[s].position.x - pos.x);
        long dist_y = abs(sensors[s].position.y - pos.y);
        long dist = dist_x + dist_y + 1;
        if (dist <= radiuses[s]) {
            return true;
        }
    }
    return false;
}

static long occupied_positions(Sensor *sensors, int sensors_len, int y) {
    // 1. Find far most left coordinate and far most coordinate for occupied beacons
    // 2. Start iterating over from found left to right
    // 3. Check if if any current tile is in range of any of the beacons (if it is, increment occupied by one)
    long min = INT64_MAX, max = INT64_MIN;
    long *radiuses = malloc(sensors_len * sizeof(long));
    for (int i = 0; i < sensors_len; i++) {
        long dist_x = abs(sensors[i].position.x - sensors[i].closest_beacon.x);
        long dist_y = abs(sensors[i].position.y - sensors[i].closest_beacon.y);
        long dist = dist_x + dist_y + 1;
        radiuses[i] = dist;

        long offset_x = dist - abs(sensors[i].position.y - y);
        int new_min = sensors[i].position.x - offset_x;
        if (new_min < min) {
            min = new_min;
        }

        int new_max = sensors[i].position.x + offset_x;
        if (new_max > max) {
            max = new_max;
        }
        // if (sensors[i].position.x - dist < min)
        //     min = sensors[i].position.x - dist;
        //
        // if (sensors[i].position.x + dist > max)
        //     max = sensors[i].position.x + dist;
    }

    // printf("%li %li\n", min, max);
    long occupied = 0;
    long new_min = min;
    while (new_min < max) {
        // 10000x speedup - WORK IN PROGRESS
        for (int s = 0; s < sensors_len; s++) {
            long dist_x = sensors[s].position.x - new_min;
            long dist_y = sensors[s].position.y - y;
            long dist = labs(dist_x) + labs(dist_y) + 1;
            if (dist <= radiuses[s]) {
                long offset_x = radiuses[s] - labs(dist_y);
                long skip = dist_x + offset_x;
                // printf(
                //     "s: %i, x: %i, y: %i, dy: %li, dx: %li, d: %li, r: %li, off: %li, skip: %li\n",
                //     s, p.x, p.y, dist_y, dist_x, dist, radiuses[s], offset_x, skip
                // );
                // if (skip > 0) {
                //     occupied += skip;
                //     new_min += skip - 1;
                // } else {
                //     assert(false && "Unreachable code detected?");
                //     occupied++;
                // }

                occupied += skip;
                new_min += skip - 1;
                break;
            };
        }
        new_min++;
    }

    free(radiuses);

    // TODO: Clean this up
    int *checked_indexes =  malloc(sensors_len * sizeof(int));
    int checked_len = 0;
    for (int s = 0; s < sensors_len; s++) {
        if (sensors[s].position.x >= min && sensors[s].position.x <= max && 
            sensors[s].position.y == y) {
            occupied--;
        }

        Pos beacon = sensors[s].closest_beacon;
        bool was_checked = false;
        for (int c = 0; c < checked_len; c++) {
            int i = checked_indexes[c];
            if (sensors[i].closest_beacon.x == beacon.x && sensors[i].closest_beacon.y == beacon.y) {
                was_checked = true;
                break;
            }
        }

        if (!was_checked && sensors[s].closest_beacon.x >= min && 
            sensors[s].closest_beacon.x <= max && sensors[s].closest_beacon.y == y) {
            checked_indexes[checked_len++] = s;
            occupied--;
        }
    }

    free(checked_indexes);

    // for (int i = min; i < max; i++) {
    //     Pos p = { .x = i, .y = y, };
    //     if (is_occupied(sensors, sensors_len, radiuses, p)) {
    //         occupied++;
    //     }
    // }
    return occupied;
}

// TODO: Skip by jumps
static Pos find_distress_beacon(Sensor *sensors, int sensors_len, int max) {

    long *radiuses = malloc(sensors_len * sizeof(long));
    for (int i = 0; i < sensors_len; i++) {
        long x_dist = abs(sensors[i].position.x - sensors[i].closest_beacon.x);
        long y_dist = abs(sensors[i].position.y - sensors[i].closest_beacon.y);
        long dist = x_dist + y_dist + 1;
        radiuses[i] = dist;
    }

    Pos beacon_pos;
    for (int i = 0; i < sensors_len; i++) {
        // top -> right -> down -> left
        Pos top = {
            .x = sensors[i].position.x,
            .y = sensors[i].position.y - radiuses[i],
        };

        Pos right = {
            .x = sensors[i].position.x + radiuses[i],
            .y = sensors[i].position.y,
        };

        Pos down = {
            .x = sensors[i].position.x,
            .y = sensors[i].position.y + radiuses[i],
        };

        Pos left = {
            .x = sensors[i].position.x - radiuses[i],
            .y = sensors[i].position.y,
        };

        
        Pos temp = top;
        while (temp.x != right.x && temp.y != right.y && temp.x >= 0 && temp.y >= 0 && temp.x <= max && temp.y <= max) {
            // draw_map_with_current_check(sensors, sensors_len, temp, i);
            if (!is_occupied(sensors, sensors_len, radiuses, temp)) {
                free(radiuses);
                return temp;
            }
            temp.y++;
            temp.x++;
        }
        
        temp = right;
        while (temp.x != down.x && temp.y != down.y && temp.x >= 0 && temp.y >= 0 && temp.x <= max && temp.y <= max) {
            // draw_map_with_current_check(sensors, sensors_len, temp, i);
            if (!is_occupied(sensors, sensors_len, radiuses, temp)) {
                free(radiuses);
                return temp;
            }
            temp.y++;
            temp.x--;
        }
        
        temp = down;
        while (temp.x != left.x && temp.y != left.y && temp.x >= 0 && temp.y >= 0 && temp.x <= max && temp.y <= max) {
            // draw_map_with_current_check(sensors, sensors_len, temp, i);
            if (!is_occupied(sensors, sensors_len, radiuses, temp)) {
                free(radiuses);
                return temp;
            }
            temp.y--;
            temp.x--;
        }
        
        temp = left;
        while (temp.x != top.x && temp.y != top.y && temp.x >= 0 && temp.y >= 0 && temp.x <= max && temp.y <= max) {
            // draw_map_with_current_check(sensors, sensors_len, temp, i);
            if (!is_occupied(sensors, sensors_len, radiuses, temp)) {
                free(radiuses);
                return temp;
            }
            temp.y--;
            temp.x++;
        }
    }

    assert(false && "Something went wrong. Is the input correct?");
}

// TODO: Part 2 needs to optimized.
void day15(FILE *input) { 
    const int sensors_cap = 32;
    Sensor sensors[sensors_cap];
    int sensors_len = 0;

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), input)) {
        Sensor s = parse_sensor(buffer);
        // printf(
        //     "%i pos %i %i, closest %i %i\n", 
        //     sensors_len, s.position.x, s.position.y,
        //     s.closest_beacon.x, s.closest_beacon.y
        // );
        assert(sensors_len != sensors_cap && "sensors out of bounds");
        sensors[sensors_len++] = s; 
    }

#if NCURSES
    // You better not use if for the large input O.o
    draw_map(sensors, sensors_len);
    for (int i = 0; i < sensors_len; i++)
        draw_map_one_sensor(sensors, sensors_len, i);
    draw_map_with_occupation(sensors, sensors_len);
#endif

#if DUMMY
    const int y = 10;
    const int max = 20;
#else
    const int y = 2000000;
    const int max = 4000000;
#endif

    // long occupied = occupied_positions(sensors, sensors_len, y);
    long occupied = occupied_positions(sensors, sensors_len, y);
    Pos beacon = find_distress_beacon(sensors, sensors_len, max);
    // Pos beacon = {};

    printf("PART 1: Positions (for y = %i) that cannot contain a beacon = %li\n", y, occupied);
    printf(
        "PART 2: Distress signal (for beacon x = %i, y = %i) is = %li\n",
        beacon.x, beacon.y, (long)beacon.x * 4000000 + beacon.y
    );
}
