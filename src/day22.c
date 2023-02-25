#include "all_days.h"

// Assumung that 1 chunk has NxN size 
int get_biggest_chunk_size() {
    assert(false);
}

int get_line_length(FILE *input) {
    int length = 0;
    char c = 0;

    while (c != '\n' && c != EOF) {
        c = getc(input);
        length += 1;
    }

    return length;
}

void day22(FILE *input) { 
    int line_length = get_line_length(input);
    printf("%i\n", line_length);
}
