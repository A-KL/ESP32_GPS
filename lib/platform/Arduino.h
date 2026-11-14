#pragma once

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

#ifndef CUSTOM_ANALOG_WRITE
inline void analogWrite(int pin, int value)
{
    log_i("analogWrite on %d with %d\n", value);
}
#endif

#ifndef CUSTOM_DIGITAL_WRITE
inline void digitalWrite(int pin, int value)
{
    log_i("digitalRead %d set as %d\n", pin, value);
}
#endif

#ifndef CUSTOM_MILLIS
long long millis()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
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

unsigned long esp_spiram_get_size()
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