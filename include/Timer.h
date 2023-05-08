#ifndef TIMER_H
#define TIMER_H

#include "all_days.h"

#if defined(__linux)
    typedef struct {
        struct timespec time;
    } Timer;
#elif defined(_WIN32)
#include <windows.h>
    typedef struct {
        LARGE_INTEGER time;
    } Timer;
#else // Other platforms, lower precision
    typedef struct {
        clock_t time;
    } Timer;
#endif

void timer_init();
Timer timer_new();
void timer_start(Timer *timer);
void timer_stop(Timer *timer);
u64 timer_elapsed(const Timer timer);
void timer_print(const char* prompt, Timer timer);

#endif // TIMER_H
