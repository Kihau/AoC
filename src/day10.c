#include "all_days.h"

static void debug_print(int reg, int cycle, char crt[240]) {
    return;

    if (cycle < 200 && cycle > INT32_MAX)
        return;
    // Sprite position: ###.....................................
    //
    //    1: begin executing addx 15
    // During cycle  1: CRT draws pixel in position 0
    // Current CRT row: #

    printf("Register:\t %i\n", reg);
    printf("Start cycle:\t %i\n", cycle);

    printf("Sprite position: ");
    for (int i = 0; i < 40; i++) {
        if (reg == i || reg == i - 1 || reg == i + 1)
            printf("#");
        else printf(".");
    }
    printf("\n");

    printf("Current cycle:   ");
    for (int j = 0; j < 40; j++) {
        printf("%c", (cycle - 1) % 40 == j ? '+' : '.');
    }
    printf("\n");

    int row  = (cycle - 1) / 40;
    printf("row %i\n", row);
    printf("Current CRT row: ");
    for (int j = 0; j < (cycle < 40 ? cycle : 40); j++) {
        char c = crt[row * 40 + j];
        printf("%c", c ? c : '.');
    }
    printf("\n\n");
}

void day10(FILE *input) { 
    char buffer[32];

    int cycle = 1, reg = 1;
    int sum_of_signals = 0;
    int step = 20;

    char crt_display[240] = { 0 };

    while (fgets(buffer, sizeof(buffer), input)) {
        // Execute first cycle of either noop or the addx instruction.
        // -----------------------------------------------------------
        
        // If sprite position (register value) alligns with current cycle, print it to the screen
        int pos = (cycle - 1) % 40;
        if (pos == reg - 1 || pos == reg || pos == reg + 1) {
            crt_display[cycle - 1] = '#';
        }
        debug_print(reg, cycle, crt_display);

        // Gather signal strengths for cycles 20, 60, 100, 140, 180, 220
        if (cycle == step) {
            int sum = reg * cycle;
            sum_of_signals += sum;
            step += 40;
        } 
        cycle++;


        switch (buffer[0]) {
            case 'a': {
                // Execute second cycle of the addx instruction.
                // ----------------------------------------------
                
                // If sprite position (register value) alligns with current cycle, print it to the screen
                int pos = (cycle - 1) % 40;
                if (pos == reg - 1 || pos == reg || pos == reg + 1) {
                    crt_display[cycle - 1] = '#';
                }
                debug_print(reg, cycle, crt_display);

                // Gather signal strengths for cycles 20, 60, 100, 140, 180, 220
                if (cycle == step) {
                    int sum = reg * cycle;
                    sum_of_signals += sum;
                    step += 40;
                } 
                cycle++;

                int value = atoi(buffer + 5);
                reg += value;
            } break;
        }
    }

    printf("PART 1: Sum of signals = %i\n", sum_of_signals);
    printf("PART 2: CRT display view:\n");
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 40; j++) {
            char c = crt_display[i * 40 + j];
            printf("%c", c ? c : '.');
        }
        printf("\n");
    }
}
