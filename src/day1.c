#include "all_days.h"

void day1(FILE *input) {
    char buffer[32];
    int curr_pos = 0, last_pos = 0;

    int sum = 0;
    int sum_max1 = 0, sum_max2 = 0, sum_max3 = 0;

    // Assumes that input is correct.
    while (fgets(buffer, sizeof(buffer), input)) {
        curr_pos = ftell(input);
        // Single, new-line character.
        if (curr_pos - last_pos == 1) {
            if (sum > sum_max1) {
                sum_max3 = sum_max2;
                sum_max2 = sum_max1;
                sum_max1 = sum; 
            } else if (sum > sum_max2) {
                sum_max3 = sum_max2;
                sum_max2 = sum;
            } else if (sum > sum_max3) {
                sum_max3 = sum;
            }

            sum = 0;
        } else {
            sum += atoi(buffer);
        }
        last_pos = curr_pos;
    }

    // There is no new line after the last elf, so curr_pos - last_pos == 1 is always false
    // (though this check was not really needed for the input provided in AoC day 1)
    if (sum > sum_max1) {
        sum_max3 = sum_max2;
        sum_max2 = sum_max1;
        sum_max1 = sum; 
    } else if (sum > sum_max2) {
        sum_max3 = sum_max2;
        sum_max2 = sum;
    } else if (sum > sum_max3) {
        sum_max3 = sum;
    }

    printf("PART1: Calories max for 1 elf = %i\n", sum_max1);
    printf("PART2: Calories max for 3 elfes = %i\n", sum_max1 + sum_max2 + sum_max3);
}
