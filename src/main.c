#include "Timer.h"
#include "Stacktrace.h"
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

void signal_handler(i32 signal) {
    fprintf(stderr, "\n");
    
    switch (signal) {
        // Segmentation fault
        case SIGSEGV: {
            fprintf(stderr, "ERROR: Segmentation fault\n");
        } break;

        // Illegal instruction
        case SIGILL: {
            fprintf(stderr, "ERROR: Illegal instruction\n");
        } break;

        // Floating point error
        case SIGFPE: {
            fprintf(stderr, "ERROR: Floating point exception\n");
        } break;

        // Program abort
        // TODO: Something fishy is going on here... 
        //       ex. segfault when running day 22 with wrong width and height, expected only abort
        case SIGABRT: {
            fprintf(stderr, "ERROR: Program aborted.\n");
        } break;

        // Don't exit the program on sigint, finalize stuff, print benchmarks and then exit
        // case SIGINT: {
        //     printf("testing\n");
        //     // Do stuff and exit program here
        //     // return;
        // } break;

        default: assert(false && "Unreachable");
    }

    StacktraceHandle trace = stacktrace_get();
    char *trace_string = stacktrace_to_string_pad(trace, "    ");

    fprintf(stderr, "Stack trace:\n");
    fprintf(stderr, "%s", trace_string);

    free(trace);
    free(trace_string);

    exit(EXIT_FAILURE);
}

void run_one_day(const char *path, i32 day_number) {
    int number_of_days = sizeof(days) / sizeof(*days);
    assert(
        day_number - 1 < number_of_days && day_number > 0 &&
        "Wrong day number. Index out of bounds."
    );
    
    printf("\n--- DAY %i ---\n", day_number);
    char path_buffer[64];
    i32 res = snprintf(path_buffer, sizeof(path_buffer), "%s/input%i.txt", path, day_number);
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

void print_help(char *program_name) {
    printf("\nAdvent of Code 2022 solutions by Kihau.\n");
    printf("\n");
    printf("Arguments:\n");
    printf("    -p, --path [dir]    path to input directory\n");
    printf("    -a, --all           run all days\n");
    printf("    -h, --help          display this prompt\n");
    // printf("    -d, --day [num]     day number\n");
    printf("\n");
    printf("Examples:\n");
    printf("-- Run day 22 from ./inputs/kihau/input22.txt:\n");
    printf("    %s -p inputs/kihau 22\n", program_name);
    printf("-- Run all days with default inputs (./inputs/dummy):\n");
    printf("    %s -a\n", program_name);
    printf("\n");
}

// NOTICE:
//     This code assumes that the inputs are CORRECT and UNMODIFIED. 
//     Any incorrect input might cause an unexpected crash of the program.
int main(int argc, char **argv) {
    signal(SIGSEGV, signal_handler);
    signal(SIGILL,  signal_handler);
    signal(SIGFPE,  signal_handler);
    signal(SIGABRT, signal_handler);

    timer_init();
    Timer total = timer_new();

    // Assuming first argument is program name
    char *program_name = argv[0];

    // Remove program name
    argc -= 1;
    argv += 1;

    // When argc is zero, run print_help and exit the program
    if (argc == 0) {
        print_help(program_name);
        return EXIT_SUCCESS;
    }

    bool run_all = false;
    int day_number = 0;
    const char *path = "inputs/dummy";

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--path")) {
            // Expect path to input as a next arguments
            i += 1;
            assert(i < argc && "You must provide path to an input file");
            path = argv[i];

        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            print_help(program_name);
            return EXIT_SUCCESS;

        } else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--all")) {
            run_all = true;

        } else {
            // If other flags are not found, input is the day number
            day_number = atoi(argv[i]);
        }
    }

    if (run_all) {
        run_all_days(path);
    } else {
        run_one_day(path, day_number);
    }

    timer_stop(&total);
    timer_print("\n------\nFinished in ", total);

    return EXIT_SUCCESS;
}
