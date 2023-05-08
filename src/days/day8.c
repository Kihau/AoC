#include "all_days.h"

#define SIZE 99

typedef struct {
    int x;
    int y;
} Vec;

typedef struct {
    int visible_trees_count;
    bool visible_from_outside;
} SearchResult;

static SearchResult search_down(char buffer[SIZE][SIZE], Vec pos) {
    SearchResult search = { 0, true };
    int pos_size = buffer[pos.y][pos.x];

    for (int y = pos.y + 1; y < SIZE; y++) {
        search.visible_trees_count += 1;
        if (pos_size <= buffer[y][pos.x]) {
            search.visible_from_outside = false;
            return search;
        } 
    }
    return search;
}

static SearchResult search_up(char buffer[SIZE][SIZE], Vec pos) {
    SearchResult search = { 0, true };
    int pos_size = buffer[pos.y][pos.x];

    for (int y = pos.y - 1; y >= 0; y--) {
        search.visible_trees_count += 1;
        if (pos_size <= buffer[y][pos.x]) {
            search.visible_from_outside = false;
            return search;
        } 
    }
    return search;
}

static SearchResult search_right(char buffer[SIZE][SIZE], Vec pos) {
    SearchResult search = { 0, true };
    int pos_size = buffer[pos.y][pos.x];

    for (int x = pos.x + 1; x < SIZE; x++) {
        search.visible_trees_count += 1;
        if (pos_size <= buffer[pos.y][x]) {
            search.visible_from_outside = false;
            return search;
        } 
    }
    return search;
}

static SearchResult search_left(char buffer[SIZE][SIZE], Vec pos) {
    SearchResult search = { 0, true };
    int pos_size = buffer[pos.y][pos.x];

    for (int x = pos.x - 1; x >= 0; x--) {
        search.visible_trees_count += 1;
        if (pos_size <= buffer[pos.y][x]) {
            search.visible_from_outside = false;
            return search;
        } 
    }
    return search;
}

void day8(FILE *input) { 
    char buffer[SIZE][SIZE];
    int temp = 0;
    while (fread(buffer[temp], 1, SIZE, input)) {
        char c;
        fread(&c, 1, 1, input);
        temp++;
    }

    int visible = 0;
    int max_viewing_score = 0;

    Vec cur;
    for (cur.y = 1; cur.y < SIZE - 1; cur.y++) {
        for (cur.x = 1; cur.x < SIZE - 1; cur.x++) {
            SearchResult down = search_down(buffer, cur);
            SearchResult up = search_up(buffer, cur);
            SearchResult right = search_right(buffer, cur);
            SearchResult left = search_left(buffer, cur);

            int viewing_score = up.visible_trees_count * right.visible_trees_count * 
                left.visible_trees_count * down.visible_trees_count;
            if (viewing_score > max_viewing_score)
                max_viewing_score = viewing_score;

            bool visible_from_outside = up.visible_from_outside || down.visible_from_outside ||
                right.visible_from_outside || left.visible_from_outside;
            if (visible_from_outside)
                visible += 1;

        }
    }
    printf("PART1: Visible = %i\n", visible + SIZE * SIZE - (SIZE - 2) * (SIZE - 2));
    printf("PART2: Viewing score = %i\n", max_viewing_score);
}
