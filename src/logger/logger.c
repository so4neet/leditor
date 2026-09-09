#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../global.h"

static void log_message(log_lvl level, const char* msg, va_list args) {
    const char* ansi_clear[4] = {"0;41", "1;31", "1;33", "1;34"};
    const char* lvl_string[4] = {"[FATAL]: ", "[ERROR]: ", "[WARNING]: ", "[DEBUG]: "};
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), msg, args);
    printf("\033[%sm%s%s\033[0m\n", ansi_clear[level], lvl_string[level], buffer);
}

void l_fatal(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    log_message(fatal, msg, args);
    va_end(args);
}

void l_error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    log_message(error, msg, args);
    va_end(args);
}

void l_warn(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    log_message(warn, msg, args);
    va_end(args);
}

void l_debug(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    log_message(debug, msg, args);
    va_end(args);
}
