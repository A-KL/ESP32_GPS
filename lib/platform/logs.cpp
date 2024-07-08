#ifndef ARDUINO

//#include <time.h>
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

unsigned long esp_get_free_heap_size()
{
    return 0;
}

unsigned long  esp_get_minimum_free_heap_size()
{
    return 0;
}

unsigned long  heap_caps_get_largest_free_block(int block)
{
    return 0;
}

unsigned long  uxTaskGetStackHighWaterMark(void* arg)
{
    return 0;
}

long long millis()
{
    // std::chrono::milliseconds uptime(0u);
    // struct timespec ts;
    // if (clock_gettime(CLOCK_UPTIME_RAW, &ts) == 0)
    // {
    // uptime = std::chrono::milliseconds(
    //     static_cast<unsigned long long>(ts.tv_sec)*1000ULL + 
    //     static_cast<unsigned long long>(ts.tv_nsec)/1000000ULL
    // );
    // }

    // #include <windows>
    // // ...
    // auto uptime = std::chrono::milliseconds(GetTickCount64());

    std::chrono::milliseconds uptime(0u);
    double uptime_seconds;

    if (std::ifstream("/proc/uptime", std::ios::in) >> uptime_seconds)
    {
        return std::chrono::milliseconds(static_cast<unsigned long long>(uptime_seconds*1000.0)).count();
    }
    return -1;
    // auto now = std::chrono::system_clock::now();
    // auto duration = now.time_since_epoch();
    // auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    // return milliseconds;
}

#endif