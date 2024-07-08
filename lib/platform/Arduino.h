#pragma once

#ifndef Arduino

#include <chrono>
#include <logs.h>

#define LOW 0
#define HIGH 1

#define INPUT_PULLUP 0
#define OUTPUT 1

inline void pinMode(int pin, int mode)
{
    log_i("Pin %d set as %d\n", pin, mode);
}

inline char digitalRead(int pin)
{
    log_i("digitalRead on %d\n", pin);

    return 0;
}

inline void digitalWrite(int pin, int value)
{
    log_i("digitalRead %d set as %d\n", pin, value);
}

// long long millis()
// {
//     auto now = std::chrono::system_clock::now();
//     auto duration = now.time_since_epoch();
//     auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
//     return milliseconds;
// }

// ESP32

// #define MALLOC_CAP_8BIT 1

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