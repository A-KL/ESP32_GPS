#pragma once

void log_d(const char* format, ...);

void log_e(const char* format, ...);

void log_v(const char* format, ...);

void log_i(const char* format, ...);

#define MALLOC_CAP_8BIT 1

unsigned long esp_get_free_heap_size();
unsigned long  esp_get_minimum_free_heap_size();
unsigned long  heap_caps_get_largest_free_block(int);
unsigned long  uxTaskGetStackHighWaterMark(void*);

long long millis();