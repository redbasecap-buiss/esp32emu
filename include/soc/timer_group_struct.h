#pragma once
#include <cstdint>
// esp32emu: soc/timer_group_struct.h stub
// Provides empty struct for code that references TIMERG0/TIMERG1 hardware registers.

typedef struct {
    struct {
        struct {
            uint32_t val;
        } config;
    } hw_timer[2];
} timg_dev_t;

// In real ESP-IDF these are memory-mapped; here just static stubs
inline timg_dev_t &TIMERG0_ref() { static timg_dev_t t{}; return t; }
inline timg_dev_t &TIMERG1_ref() { static timg_dev_t t{}; return t; }
#define TIMERG0 (TIMERG0_ref())
#define TIMERG1 (TIMERG1_ref())
