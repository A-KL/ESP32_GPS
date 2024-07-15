#pragma once

#ifndef Arduino

#include <logs.h>
#include <fstream>
#include <chrono>
#include <stdio.h>
#include <stdarg.h>

#define LOW 0
#define HIGH 1

#define INPUT_PULLUP 0
#define OUTPUT 1


#ifndef CUSTOM_DELAY
inline void delay(uint32_t msec)
{
    log_i("Delay for %d msec\n", msec);
}
#endif

#ifndef CUSTOM_PIN_MODE
inline void pinMode(int pin, int mode)
{
    log_i("Pin %d set as %d\n", pin, mode);
}
#endif

#ifndef CUSTOM_DIGITAL_READ
inline char digitalRead(int pin)
{
    log_i("digitalRead on %d\n", pin);

    return 0;
}
#endif

inline void analogWrite(int pin, int value)
{
    log_i("analogWrite on %d with %d\n", value);
}

#ifndef CUSTOM_DIGITAL_WRITE
inline void digitalWrite(int pin, int value)
{
    log_i("digitalRead %d set as %d\n", pin, value);
}
#endif

#ifndef CUSTOM_MILLIS
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

// ESP32

// #define MALLOC_CAP_8BIT 1

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

// unsigned long esp_get_free_heap_size()
// {
//     return 0;
// }

// unsigned long  esp_get_minimum_free_heap_size()
// {
//     return 0;
// }

// unsigned long  heap_caps_get_largest_free_block(int block)
// {
//     return 0;
// }

// unsigned long  uxTaskGetStackHighWaterMark(void* arg)
// {
//     return 0;
// }

#endif