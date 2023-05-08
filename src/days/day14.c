#include "all_days.h"

typedef struct {
    int x;
    int y;
} Pos;

typedef enum {
    ROCK = '#',
    SAND = 'o',
    AIR  = '.',
} Tile;

static Pos parse_coord(char buffer[312], int *offset) {
    Pos coords = { -1, -1 };

    int value_index = 0;
    char value_buf[8] = {};
    while (buffer[*offset] != ',') {
        value_buf[value_index++] = buffer[*offset];
        (*offset)++;
    }
    (*offset)++;

    value_buf[value_index] = 0;
    coords.x = atoi(value_buf);

    value_index = 0;
    while (buffer[*offset] != ' ' && buffer[*offset]) {
        value_buf[value_index++] = buffer[*offset];
        (*offset)++;
    }

    value_buf[value_index] = 0;
    coords.y = atoi(value_buf);

    return coords;
}

static void parse_line(char buffer[312], int *offset, Pos *min, Pos *max) {
    while (buffer[*offset]) {
        Pos p = parse_coord(buffer, offset);
        // printf("%i %i, ", test.x, test.y);
        if (p.x < min->x)
            min->x = p.x;

        if (p.y < min->y)
            min->y = p.y;

        if (p.x > max->x)
            max->x = p.x;

        if (p.y > max->y)
            max->y = p.y;


        while (buffer[*offset] != '>' && buffer[*offset]) {
            (*offset)++;
        }
        if (!buffer[*offset]) {
            return;
        }
        (*offset)++;
    }
}

static void place_rocks(char buffer[312], int *offset, char **map, Pos min, int off_x, int off_y) {
    Pos prev = { -1, -1 };
    while (buffer[*offset]) {
        Pos p = parse_coord(buffer, offset);
        Pos now = { .x = p.x - min.x, .y = p.y - min.y };

        // printf("prev %i %i, now %i %i, p %i %i\n", prev.x, prev.y, now.x, now.y, p.x, p.y);

        if (now.x == prev.x) {
            int min_y, max_y;
            if (now.y > prev.y) {
                min_y = prev.y;
                max_y = now.y;
            } else {
                min_y = now.y;
                max_y = prev.y;
            }

            for (int i = min_y; i <= max_y; i++) {
                // printf("y %i\n", now.y + i);
                map[i + min.y][now.x + off_x] = ROCK;
            }
        } else if (now.y == prev.y) {
            int min_x, max_x;
            if (now.x > prev.x) {
                min_x = prev.x;
                max_x = now.x;
            } else {
                min_x = now.x;
                max_x = prev.x;
            }

            for (int i = min_x; i <= max_x; i++) {
                // printf("x %i\n", prev.x + i);
                map[now.y + min.y][i + off_x] = ROCK;
            }
        }

        prev = now;

        // printf("%i %i, ", test.x, test.y);
        while (buffer[*offset] != '>' && buffer[*offset]) {
            (*offset)++;
        }
        if (!buffer[*offset]) {
            return;
        }
        (*offset)++;
    }
}

static int drop_sand_floor(char **map, const Pos len, const Pos original_pos) {
    int sand_count = 0;
    Pos sand_pos = original_pos;
    while (true) { 
        // Sand fell to the void
        if (map[sand_pos.y][sand_pos.x] == SAND){
            return sand_count;
        }

        if (sand_pos.y + 1 == len.y) {
            map[sand_pos.y][sand_pos.x] = SAND;
            sand_count++;
            sand_pos = original_pos;
        } else if (map[sand_pos.y + 1][sand_pos.x] == AIR) {
            // Fall down
            sand_pos.y++;
        } else if (map[sand_pos.y + 1][sand_pos.x - 1] == AIR) {
            // Fall left
            sand_pos.y++;
            sand_pos.x--;
        } else if (map[sand_pos.y + 1][sand_pos.x + 1] == AIR) {
            // Fall right
            sand_pos.y++;
            sand_pos.x++;
        } else {
            // Sand no longer moves
            map[sand_pos.y][sand_pos.x] = SAND;
            sand_count++;
            sand_pos = original_pos;
        }
    }
}

static int drop_sand_void(char **map, const Pos len, const Pos original_pos) {
    int sand_count = 0;
    Pos sand_pos = original_pos;
    while (true) { 
        // Sand fell to the void
        if (sand_pos.y + 1 >= len.y) {
            return sand_count;
        }

        if (map[sand_pos.y + 1][sand_pos.x] == AIR) {
            // Fall down
            sand_pos.y++;
        } else if (map[sand_pos.y + 1][sand_pos.x - 1] == AIR) {
            // Fall left
            sand_pos.y++;
            sand_pos.x--;
        } else if (map[sand_pos.y + 1][sand_pos.x + 1] == AIR) {
            // Fall right
            sand_pos.y++;
            sand_pos.x++;
        } else {
            // Sand no longer moves
            map[sand_pos.y][sand_pos.x] = SAND;
            sand_count++;
            sand_pos = original_pos;
        }
    }
}

static char **allocate_map(Pos len) {
    char **map = malloc(len.y * sizeof(char *));
    for (int i = 0; i < len.y; i++) {
        map[i] = malloc(len.x);
        memset(map[i], AIR, len.x);
    }

    return map;
}

static void free_map(char **map, Pos len) {
    for (int i = 0; i < len.y; i++)
        free(map[i]);
    free(map);
}

// TODO: 
//     - Add some assert bounds checks
//     - Ncursess simulation (as I usually do)
void day14(FILE *input) { 
    char buffer[312];
    int offset = 0;

    Pos min = { INT32_MAX, INT32_MAX };
    Pos max = { INT32_MIN, INT32_MIN };

    while (fgets(buffer, sizeof(buffer), input)) {
        parse_line(buffer, &offset, &min, &max);
        offset = 0;
    }

    // Offset x by 1 to the right and 1 to the left, so the sand can just fall down and not
    // go out of bounds
    // TODO: Bounds check may actually increase the perf since the sand goes out-of-bounds, it does
    //       not need to be calculated anymore and the sand dropping loop can continue to the next one.
    Pos len1 = { .x = max.x - min.x + 3, .y = max.y + 2 };
    char **map1 = allocate_map(len1);

    // Since I know the height and width (and also the start position), this probably can be
    // estimated and proper allocation can be made
    const int inf_floor = 300;
    // Pos len2 = { .x = max.x - min.x + inf_floor, .y = max.y - min.y + 17 };
    Pos len2 = { .x = max.x - min.x + inf_floor, .y = max.y + 2 };
    char **map2 = allocate_map(len2);

    // const Pos drop_pos1 = { .x = 500 - min.x + 1, .y = 0 };
    printf("min %i %i\n", min.x, min.y);
    printf("max %i %i\n", max.x, max.y);
    printf("len %i %i\n", len1.x, len1.y);
    // printf("drop pos %i %i\n", drop_pos1.x, drop_pos1.y);

    fseek(input, 0, SEEK_SET);
    while (fgets(buffer, sizeof(buffer), input)) {
        place_rocks(buffer, &offset, map1, min, 1, 0);
        offset = 0;
        place_rocks(buffer, &offset, map2, min, inf_floor / 2, 0);
        offset = 0;
    }

    // for (int y = 0; y < len1.y; y++) {
    //     for (int x = 0; x < len1.x; x++) {
    //         printf("%c", map1[y][x]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");

    // Sand drop start point: 500,0
    const Pos drop_pos1 = { .x = 500 - min.x + 1, .y = 0 };
    int sand_dropped1 = drop_sand_void(map1, len1, drop_pos1);

    for (int y = 0; y < len1.y; y++) {
        for (int x = 0; x < len1.x; x++) {
            printf("%c", map1[y][x]);
        }
        printf("\n");
    }
    printf("\n");

    const Pos drop_pos2 = { .x = 500 - min.x + inf_floor / 2, .y = 0 };
    int sand_dropped2 = drop_sand_floor(map2, len2, drop_pos2);
    
    for (int y = 0; y < len2.y; y++) {
        for (int x = 0; x < len2.x; x++) {
            printf("%c", map2[y][x]);
        }
        printf("\n");
    }
    printf("\n");

    free_map(map1, len1);
    free_map(map2, len2);

    printf("PART 1: Dropped sand = %i\n", sand_dropped1);
    printf("PART 2: Dropped sand = %i\n", sand_dropped2);
}
