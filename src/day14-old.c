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

Pos parse_coord(char buffer[312], int *offset) {
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

void parse_line(char buffer[312], int *offset, Pos *min, Pos *max) {
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

void update_map(char buffer[312], int *offset, char **map, Pos min) {
    Pos prev = { -1, -1 };
    while (buffer[*offset]) {
        Pos p = parse_coord(buffer, offset);
        Pos now = { .x = p.x - min.x, .y = p.y - min.y };

        printf("prev %i %i, now %i %i, p %i %i\n", prev.x, prev.y, now.x, now.y, p.x, p.y);

        if (now.x == prev.x) {
            int min, max;
            if (now.y > prev.y) {
                min = prev.y;
                max = now.y;
            } else {
                min = now.y;
                max = prev.y;
            }

            for (int i = min; i <= max; i++) {
                // printf("y %i\n", now.y + i);
                map[i + 6][now.x + 5] = ROCK;
            }
        } else if (now.y == prev.y) {
            int min, max;
            if (now.x > prev.x) {
                min = prev.x;
                max = now.x;
            } else {
                min = now.x;
                max = prev.x;
            }

            for (int i = min; i <= max; i++) {
                // printf("x %i\n", prev.x + i);
                map[now.y + 6][i + 5] = ROCK;
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

void drop_sand_like_a_boss() {
    while (true) { }
}

void day14(FILE *input) { 
    char buffer[312];
    int offset = 0;

    Pos min = { INT32_MAX, INT32_MAX };
    Pos max = { INT32_MIN, INT32_MIN };

    while (fgets(buffer, sizeof(buffer), input)) {
        parse_line(buffer, &offset, &min, &max);
        offset = 0;
    }

    Pos len = { .x = max.x - min.x + 11, .y = max.y - min.y + 7 };
    char **map = malloc(len.y * sizeof(char *));
    for (int i = 0; i < len.y; i++) {
        map[i] = malloc(len.x);
        memset(map[i], AIR, len.x);
    }

    fseek(input, 0, SEEK_SET);
    while (fgets(buffer, sizeof(buffer), input)) {
        update_map(buffer, &offset, map, min);
        offset = 0;
    }

    for (int y = 0; y < len.y; y++) {
        for (int x = 0; x < len.x; x++) {
            printf("%c", map[y][x]);
        }
        printf("\n");
    }

    const Pos drop_pos = { .x = 500 - min.x + 5, .y = 0 };
    // Sand drop start point: 500,0

    for (int i = 0; i < len.y; i++) {
        free(map[i]);
    }

    printf("min %i %i\n", min.x, min.y);
    printf("max %i %i\n", max.x, max.y);
    printf("size %i %i\n", max.x - min.x, max.y - min.y);
    printf("drop pos %i %i\n", drop_pos.x, drop_pos.y);
}
