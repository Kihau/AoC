#include "Stacktrace.h"

typedef struct String {
    char *buffer;
    int size;
    int cap;
} String;

static String string_new() {
    String buffer = {
        .buffer = (char *)malloc(1024), 
        .size = 0, 
        .cap = 1024 
    };
    return buffer;
}

static void string_printf(String *string, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const int len = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);

    const int new_end = string->size + len;

    if (new_end > string->cap) {
        while (new_end > string->cap) {
            string->cap *= 2;
        }
        string->buffer = (char *)realloc(string->buffer, string->cap);
    }

    va_start(args, fmt);
    string->size += vsnprintf(string->buffer + string->size, len, fmt, args);
    va_end(args);
}

char *stacktrace_get_string() {
    StacktraceHandle trace = stacktrace_get();
    char *output = stacktrace_to_string(trace);
    free(trace);
    return output;
}


#if defined(_WIN32)

StacktraceHandle stacktrace_get() {
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    CONTEXT context;
    STACKFRAME64 frame; // in/out stackframe
    DWORD imageType;
    StacktraceEntry *ret = (StacktraceEntry *)malloc(STACKTRACE_MAX_DEPTH * sizeof(StacktraceEntry));
    int i = 0;

    if (!SymInitialize_called) {
        SymInitialize(process, NULL, TRUE);
        SymInitialize_called = 1;
    }

    RtlCaptureContext(&context);

    memset(&frame, 0, sizeof(frame));
#ifdef _M_IX86
    imageType = IMAGE_FILE_MACHINE_I386;
    frame.AddrPC.Offset = context.Eip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.Ebp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.Esp;
    frame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    imageType = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrPC.Offset = context.Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.Rsp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.Rsp;
    frame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
    imageType = IMAGE_FILE_MACHINE_IA64;
    frame.AddrPC.Offset = context.StIIP;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.IntSp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrBStore.Offset = context.RsBSP;
    frame.AddrBStore.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.IntSp;
    frame.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform is not supported!"
#endif

    while (true) {
        StacktraceEntry *cur = ret + i++;
        if (i == STACKTRACE_MAX_DEPTH) {
            cur->AddrPC_Offset = 0;
            cur->AddrReturn_Offset = 0;
            break;
        }

        if (!StackWalk64(imageType, process, thread, &frame, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
            cur->AddrPC_Offset = frame.AddrPC.Offset;
            cur->AddrReturn_Offset = STACKTRACE_ERROR_FLAG; // mark error
            cur->AddrReturn_Offset |= GetLastError();
            break;
        }

        cur->AddrPC_Offset = frame.AddrPC.Offset;
        cur->AddrReturn_Offset = frame.AddrReturn.Offset;

        if (frame.AddrReturn.Offset == 0) {
            break;
        }
    }

    return (StacktraceHandle)(ret);
}

char *stacktrace_to_string(StacktraceHandle handle) {
    const StacktraceEntry *entries = (StacktraceEntry *)handle;
    int i = 0;
    HANDLE process = GetCurrentProcess();
    IMAGEHLP_SYMBOL64 *symbol = (IMAGEHLP_SYMBOL64 *)malloc(sizeof(IMAGEHLP_SYMBOL64) + 1024);

    String string = string_new();
    symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
    symbol->MaxNameLength = 1024;

    while (true) {
        const StacktraceEntry *cur = entries + i++;
        if (cur->AddrReturn_Offset & STACKTRACE_ERROR_FLAG) {
            DWORD error = cur->AddrReturn_Offset & 0xFFFFFFFF;
            string_printf(&string, "StackWalk64 error: %d @ %p\n", error, cur->AddrPC_Offset);
            break;
        }

        if (cur->AddrPC_Offset == cur->AddrReturn_Offset) {
            string_printf(&string, "Stack overflow @ %p\n", cur->AddrPC_Offset);
            break;
        }

        DWORD lineOffset = 0;
        IMAGEHLP_LINE64 lineData;
        SymGetLineFromAddr64(process, cur->AddrPC_Offset, &lineOffset, &lineData);
        string_printf(&string, "%s(%d): ", lineData.FileName, lineData.LineNumber);

        DWORD64 symOffset = 0;
        if (SymGetSymFromAddr64(process, cur->AddrPC_Offset, &symOffset, symbol)) {
            string_printf(&string, "%s\n", symbol->Name);
        } else {
            string_printf(&string, " Unkown symbol @ %p\n", cur->AddrPC_Offset);
        }

        if (cur->AddrReturn_Offset == 0) {
            break;
        }
    }

    free(symbol);
    return string.buf;
}

#elif defined(__APPLE__)

StacktraceHandle stacktrace_get() {
    Stacktrace *trace = (Stacktrace *)malloc(sizeof(Stacktrace));
    trace->trace_size = backtrace(trace->trace, STACKTRACE_MAX_DEPTH);
    return trace;
}

char *stacktrace_to_string(StacktraceHandle h) {
    const Stacktrace *Stacktrace = (Stacktrace *)h;
    char **messages = backtrace_symbols(Stacktrace->trace, Stacktrace->trace_size);
    String out = string_new();
    *out.buf = 0;

    for (int i = 0; i < Stacktrace->trace_size; ++i) {
        string_printf(&out, "%s\n", messages[i]);
    }

    free(messages);
    return out.buf;
}

#elif defined(__linux__)

StacktraceHandle stacktrace_get() {
    Stacktrace *stack = (Stacktrace *)malloc(sizeof(Stacktrace));
    stack->trace_size = backtrace(stack->trace, STACKTRACE_MAX_DEPTH);
    return stack;
}

char *stacktrace_to_string(StacktraceHandle handle) {
    const Stacktrace *stack = handle;
    char **messages = backtrace_symbols(stack->trace, stack->trace_size);
    String output = string_new();

    for (int i = 0; i < stack->trace_size; ++i) {
        void *tracei = stack->trace[i];
        char *msg = messages[i];

        // Calculate the load offset.
        Dl_info info;
        dladdr(tracei, &info);
        if (info.dli_fbase == (void *)0x400000) {
            // Don't offset address from the executable.
            info.dli_fbase = NULL;
        }

        while (*msg && *msg != '(') {
            ++msg;
        }
        *msg = 0;

        {
            char cmd[1024];
            snprintf(
                cmd, 1024, "addr2line -e %s -f -C -p %p 2>/dev/null", messages[i],
                // ?????????????????
                (void *)((char *)tracei - (char *)info.dli_fbase)
            );

            FILE *fp = popen(cmd, "r");
            if (!fp) {
                string_printf(&output, "Failed to generate trace further...\n");
                break;
            }

            char line[2048];
            while (fgets(line, sizeof(line), fp)) {
                string_printf(&output, "%s: ", messages[i]);
                if (strstr(line, "?? ")) {
                    // Output address if nothing was found.
                    string_printf(&output, "%p\n", tracei);
                } else {
                    string_printf(&output, "%s", line);
                }
            }

            pclose(fp);
        }
    }

    free(messages);
    return output.buffer;
}

#endif
