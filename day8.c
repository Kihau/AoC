#include "all_days.h"

#define SIZE 99

void day8(FILE *input) { 
    char buffer[SIZE][SIZE];
    int temp = 0;
    while (fread(buffer[temp], 1, SIZE, input)) {
        char c;
        fread(&c, 1, 1, input);
        temp++;
    }
    int n = SIZE, m = SIZE;

    // printf("%s", buffer);
    int visible = 0;
    int blocking = 0;
    int max_viewing_score = 0;
    for (int i = 1; i < n - 1; i++) {
        for (int j = 1; j < m - 1; j++) {
            bool found = false;
            int len = buffer[i][j];
            // printf("%i", buffer[i][j]);

            int down = 0;
            blocking = 0;
            for (int k = i + 1; k < m; k++) {
                if (len <= buffer[k][j]) {
                    blocking++;
                    down++;
                    break;
                } else down++;
            }
            if (blocking == 0 && !found) {
                found = true;
                visible++;
            }

            blocking = 0;
            int right = 0;
            for (int k = j + 1; k < m; k++) {
                if (len <= buffer[i][k]) {
                    blocking++;
                    right++;
                    break;
                } else right++;
            }
            if (blocking == 0 && !found) {
                visible++;
                found = true;
            }

            blocking = 0;
            int up = 0;
            for (int k = i - 1; k >= 0; k--) {
                if (len <= buffer[k][j]) {
                    blocking++;
                    up++;
                    break;
                } else up++;
            }
            if (blocking == 0 && !found) {
                visible++;
                found = true;
            }

            blocking = 0;
            int left = 0;
            for (int k = j - 1; k >= 0; k--) {
                if (len <= buffer[i][k]) {
                    blocking++;
                    left++;
                    break;
                } else left++;
            }
            if (blocking == 0 && !found) {
                visible++;
                found = true;
            }
            
            int viewing_score = up * right * left * down;
            if (viewing_score > max_viewing_score)
                max_viewing_score = viewing_score;
        }
    }
    printf("PART1: Visible = %i\n", visible + SIZE * SIZE - (SIZE - 2) * (SIZE - 2));
    printf("PART2: Viewing score = %i\n", max_viewing_score);
}
