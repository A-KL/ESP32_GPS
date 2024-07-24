#ifndef ARDUINO

#include <fstream>
#include <chrono>
#include <stdio.h>
#include <stdarg.h>
#include "logs.h"

void log_d(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);  
}

void log_e(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf (format, args);
    va_end(args);
}

void log_v(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void log_i(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

#endif