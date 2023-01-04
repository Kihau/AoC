#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

static char find_item(char *comp1, char *comp2, int size) {
    int comp_len = size / 2;
    for (int i = 0; i < comp_len; i++) {
        char item = comp1[i];
        for (int ii = 0; ii < comp_len; ii++) {
            if (item == comp2[ii]) return item;
        }
    }
    assert(false && "Incorrect input, item not found");
}


static char find_badge(char *comp1, int size1, char *comp2, int size2, char *comp3, int size3) {
    for (int i = 0; i < size1; i++) {
        char badge = comp1[i];
        bool found_in_second = false;
        for (int ii = 0; ii < size2; ii++) {
            if (badge == comp2[ii]) found_in_second = true;
        }

        if (!found_in_second)
            continue;

        for (int ii = 0; ii < size3; ii++) {
            if (badge == comp3[ii]) return badge;
        }
    }
    assert(false && "Incorrect input, badge not found");
}

void day3(FILE *input) {
    char rucksack[3][128];
    int sizes[3];
    
    int curr_pos = 0, last_pos = 0;

    int priorities_items = 0;
    int priorities_badges = 0;

    bool found_eof = false;
    while (!found_eof) {
        for (int i = 0; i < 3; i++) {
            if (!fgets(rucksack[i], sizeof(rucksack[i]), input)) {
                found_eof = true;
                break;
            }

            curr_pos = ftell(input);
            sizes[i] = curr_pos - last_pos - 1;

            last_pos = curr_pos;

            int item = find_item(rucksack[i], rucksack[i] + sizes[i] / 2, sizes[i]);

            // a = 97
            // z = 122
            // A = 65
            // Z = 90
            // TODO: ????????????????????
            if (item >= 'a')
                priorities_items += item - 'a' + 1;
            else priorities_items += item - 'Z' + 26 + 26;
        }

        int badge = find_badge(
            rucksack[0], sizes[0], rucksack[1], sizes[1], rucksack[2], sizes[2]
        );

        if (badge >= 'a')
            priorities_badges += badge - 'a' + 1;
        else priorities_badges += badge - 'Z' + 26 + 26;
    } 

    printf("Part 1: Priorities = %i\n", priorities_items);
    printf("Part 2: Priorities = %i\n", priorities_badges);
}
