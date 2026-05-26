#include "base.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef GAME_DEBUG
    fn _logf_function(const char* fmt, ...) -> void {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf("\n");
    }
#endif

fn _checkf_function(bool expr, const char* fmt, ...) -> void {
    if (!expr) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf("\n");
        dbg_break();
    }
}

fn _ensuref_function(bool expr, const char* fmt, ...) -> bool {
    if (!expr) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf("\n");
        dbg_break();
    }
    return expr;
}