#include "all_days.h"

typedef struct {
    int min;
    int max;
} Range;

static Range parse_range(char *range_string, int size) {
    int split;
    for (split = 0; split < size; split++) {
        if (range_string[split] == '-')
            break;
    }
    char min[8] = {};
    memcpy(min, range_string, split);

    char max[8] = {};
    memcpy(max, range_string + split + 1, size - split - 1);

    Range range = {
        .min = atoi(min),
        .max = atoi(max),
    };
    return range;
}

static bool ranges_fulloverlap(Range r1, Range r2) {
    //  min2      min1      max1      max2
    //   |---------|#########|---------|
    if (r2.min <= r1.min && r1.max <= r2.max) {
        return true;
    //        min1       min2       max2      max1
    //         |----------|##########|---------|
    } else if (r1.min <= r2.min && r2.max <= r1.max) {
        return true;
    } else return false;
}

static bool ranges_anyoverlap(Range r1, Range r2) {
    //     max1  min2
    // |----|     |------|
    if (r1.max < r2.min)
        return false;
    //          max2  min1
    //  |--------|     |-------|
    else if (r2.max < r1.min)
        return false;
    else return true;
}

void day4(FILE *input) {
    char buffer[32];

    int fulloverlap_count = 0;
    int anyoverlap_count = 0;
    int curr_pos = 0, last_pos = 0;
    while (fgets(buffer, sizeof(buffer), input)) {
        curr_pos = ftell(input);
        int size = curr_pos - last_pos - 1;

        int split;
        for (split = 0; split < size; split++) {
            if (buffer[split] == ',')
                break;
        }

        Range r1 = parse_range(buffer, split);
        Range r2 = parse_range(buffer + split + 1, size - split - 1);
        if (ranges_fulloverlap(r1, r2))
            fulloverlap_count += 1;

        if (ranges_anyoverlap(r1, r2))
            anyoverlap_count += 1;

        last_pos = curr_pos;
    }

    printf("PART 1: Full overlap count = %i\n", fulloverlap_count);
    printf("PART 2: Any overlap count = %i\n", anyoverlap_count);
}
