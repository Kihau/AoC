#include "Timer.h"
#include "all_days.h"

void (*days[]) (FILE *) = {
    day1,  day2,  day3,  day4,  day5,  day6,  day7,  day8,  day9,  day10, 
    day11, day12, day13, day14, day15, day16, day17, day18, day19, day20,
    day21, day22, //day23, day24, day25
};

// struct timespec timer_start() {
//     struct timespec time;
//     clock_gettime(CLOCK_REALTIME, &time);
//     return time;
// }
//
// struct timespec timer_stop(struct timespec time_before) {
//     struct timespec time_now;
//     clock_gettime(CLOCK_REALTIME, &time_now);
//
//     struct timespec elapsed = {
//         .tv_sec = time_now.tv_sec - time_before.tv_sec,
//         .tv_nsec = time_now.tv_nsec - time_before.tv_nsec,
//     };
//     return elapsed;
// }
//
// // This can be a macro
// void print_benchmark(const char* prompt, struct timespec elapsed) {
//     printf("%s", prompt);
//
//     double time = 0;
//     if (elapsed.tv_sec > 0) {
//         time += elapsed.tv_sec;
//         time += (double)elapsed.tv_nsec / 1000000000; 
//         printf("%lfs\n", time);
//     } else if (elapsed.tv_nsec / 1000000) {
//         time += (double)elapsed.tv_nsec / 1000000;
//         printf("%lfms\n", time);
//     } else if (elapsed.tv_nsec / 1000) {
//         time += (double)elapsed.tv_nsec / 1000;
//         printf("%lfμs\n", time);
//     } else {
//         time += (double)elapsed.tv_nsec;
//         printf("%lfns\n", time);
//     }
// }

void run_one_day(const char *path, int day_number) {
    int number_of_days = sizeof(days) / sizeof(*days);
    assert(
        day_number - 1 < number_of_days && day_number > 0 &&
        "Wrong day number. Index out of bounds."
    );
    
    printf("\n--- DAY %i ---\n", day_number);
    char path_buffer[64];
    int res = snprintf(path_buffer, sizeof(path_buffer), "%s/input%i.txt", path, day_number);
    assert(res > 0 && res < (int)sizeof(path_buffer) && "Failed to write to the buffer");

    Timer timer = timer_new();

    FILE *input_file = fopen(path_buffer, "r");
    assert(input_file != NULL && "Could not open input file");
    days[day_number - 1](input_file);
    fclose(input_file);

    timer_stop(&timer);
    timer_print("Executed in ", timer);
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
    timer_init();

    Timer total = timer_new();

    int day_number = 0;
    const char *path = "inputs/dummy";
    
    // TODO: When argc is zero, run help command and exit the program
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            // Expect path to input as a next arguments
            i += 1;
            assert(i < argc && "You must provide path to an input file");
            path = argv[i];
        } else {
            // If other flags are not found, input is the day number
            day_number = atoi(argv[i]);
        }
    }

    if (day_number != 0) {
        run_one_day(path, day_number);
    } else {
        // Debugging
        run_one_day(path, 22);

        // run_all_days(path);
    }

    timer_stop(&total);
    timer_print("\n------\nFinished in ", total);

    return 0;
}
