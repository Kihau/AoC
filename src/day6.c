#include "all_days.h"

static int dummy_solution(char *buffer, int size) {
    char fours[5] = {
        buffer[0], buffer[1], buffer[2], buffer[3], 0
    };

    int start;
    for (start = 4; start < size; start++) {
        if (
            fours[0] != fours[1] && fours[0] != fours[2] && fours[0] != fours[3] &&
            fours[1] != fours[2] && fours[1] != fours[3] && fours[2] != fours[3]  
        ) break;

        fours[0] = fours[1];
        fours[1] = fours[2];
        fours[2] = fours[3];
        fours[3] = buffer[start];
    }
    return start;
}

static int find_packet_start(char *buffer, int buffer_size, const int packet_size) {
    // TODO(?): The following code works, but buffer % packet_size may not be equal to 0
    //       It might be necessary to check the remaining data outside of this loop
    for (int i = 0; i < buffer_size - packet_size; i += 1) {
        int map = 0;
        for (int ii = i; ii < i + packet_size; ii += 1) {
            map |= 1 << (buffer[ii] - 'a') ;
        }

        int bits = 0;
        while (map) {
            bits += map & 1;
            map >>= 1;
        }

        if (bits == packet_size)
            return i + packet_size;
    }
    assert(false && "Incorrect input");
}

void day6(FILE *input) {
    char buffer[4096];
    // Load all data
    fread(buffer, 1, sizeof(buffer), input);
    // int start_dummy = dummy_solution(buffer, 4096);
    int start1 = find_packet_start(buffer, 4096, 4);
    int start2 = find_packet_start(buffer, 4096, 16);
    
    // printf("PART 1(dummy): Start of packet = %i\n", start_dummy);
    printf("PART 1: Start of packet = %i\n", start1);
    printf("PART 2: Start of packet = %i\n", start2);
}
