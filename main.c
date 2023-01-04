#include "all_days.h"

void (*days[]) (FILE *) = {
    day1, day2, day3, day4, day5, day6, day7, day8, day9, day10, 
    day11, day12, day13, day14, day15
};

void print_benchmark(const char* prompt, clock_t time) {
    printf("%s", prompt);

    if (time / 1000000)
        printf("%fs\n", (float)time / 1000000);
    else if (time / 1000)
        printf("%fms\n", (float)time / 1000);
    else printf("%liμs\n", time);
    // else if (time)
    //     printf("%liμs\n", time);
    // else 
    //     printf("%lins\n", time);
}

void run_one_day(const char* path, int day_number) {
    int number_of_days = sizeof(days) / sizeof(*days);
    assert(
        day_number - 1 < number_of_days && day_number > 0 &&
        "Wrong day number. Index out of bounds."
    );
    
    printf("\n--- DAY %i ---\n", day_number);
    char path_buffer[64];
    int res = snprintf(path_buffer, sizeof(path_buffer), "%s%i.txt", path, day_number);
    assert(res > 0 && res < (int)sizeof(path_buffer) && "Failed to write to the buffer");

    clock_t start = clock();

    FILE *input_file = fopen(path_buffer, "r");
    assert(input_file != NULL && "Could not open input file");
    days[day_number - 1](input_file);
    fclose(input_file);

    print_benchmark("Executed in ", clock() - start);
}

void run_all_days(const char *path) {
    int number_of_days = sizeof(days) / sizeof(*days);

    for (int i = 0; i < number_of_days; i++) {
        run_one_day(path, i + 1);
    }
}

// NOTICE:
//     This code assumes that the input is CORRECT and UNMODIFIED. 
//     Any incorrect input might cause an unexpected crash of the program.
int main(int argc, char **argv) {
    clock_t total_time = clock();
    const char *path = "my-inputs/input";
    // const char *path = "frisk-inputs/input";
    // const char *path = "dummy-inputs/dummy";
    
    if (argc > 1) {
        run_one_day(path, atoi(argv[1]));
    } else {
        // run_one_day(path, 14);
        run_all_days(path);
    }

    print_benchmark("\n------\nFinished in ", clock() - total_time);

    return 0;
}
