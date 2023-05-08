#ifndef STACKTRACE_H
#define STACKTRACE_H

#if defined(__linux__)
#define _GNU_SOURCE
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STACKTRACE_MAX_DEPTH 1024

// typedef struct String {
//     char *buffer;
//     int size;
//     int cap;
// } String;

/// Stacktrace handle
typedef struct Stacktrace *StacktraceHandle;

/// Returns a stack-trace handle from the point of view of the caller which
/// can be expanded to a string via stacktrace_to_string.
/// The handle is allocated with `malloc` and needs to be freed with `free`
StacktraceHandle stacktrace_get();

/// Converts a stack-trace handle to a human-readable string.
/// The string is allocated with `malloc` and needs to be freed with `free`
char *stacktrace_to_string(StacktraceHandle Stacktrace);

// TODO: Document this
// String string_new();

// TODO: Document this
// void string_printf(String *string, const char *fmt, ...);

/// Returns a human-readable stack-trace string from the point of view of the caller.
/// The string is allocated with `malloc` and needs to be freed with `free`
char *stacktrace_get_string();

#if defined(_WIN32)

#include <dbghelp.h>
#include <tlhelp32.h>
#include <windows.h>

#pragma comment(lib, "DbgHelp.lib")

#define STACKTRACE_ERROR_FLAG ((DWORD64)1 << 63)

typedef struct StacktraceEntry {
    DWORD64 AddrPC_Offset;
    DWORD64 AddrReturn_Offset;
} StacktraceEntry;

static int SymInitialize_called = 0;

#elif defined(__APPLE__)

#include <dlfcn.h>
#include <execinfo.h>
#include <unistd.h>

typedef struct Stacktrace {
    void *trace[STACKTRACE_MAX_DEPTH];
    int trace_size;
} Stacktrace;

#elif defined(__linux__)

#include <dlfcn.h>
#include <execinfo.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>

typedef struct Stacktrace {
    void *trace[STACKTRACE_MAX_DEPTH];
    int trace_size;
} Stacktrace;

#else
#error "Platform is not supported!"
// Platform is not supported yet.
// char* stacktrace_get_string() {
//     String out = string_new();
//     string_printf("Stacktrace: unsupported platform\n");
//     return out.buf;
// }
#endif // platform

#endif // STACKTRACE_H
