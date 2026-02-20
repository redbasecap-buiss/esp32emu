#pragma once
// esp32emu: soc/rtc.h stub — RTC clock definitions

typedef enum {
    RTC_CPU_FREQ_XTAL = 0,
    RTC_CPU_FREQ_80M  = 1,
    RTC_CPU_FREQ_160M = 2,
    RTC_CPU_FREQ_240M = 3,
    RTC_CPU_FREQ_2M   = 4
} rtc_cpu_freq_t;

typedef enum {
    RTC_SLOW_FREQ_RTC     = 0,
    RTC_SLOW_FREQ_32K_XTAL = 1,
    RTC_SLOW_FREQ_8MD256  = 2
} rtc_slow_freq_t;

typedef enum {
    RTC_FAST_FREQ_XTALD4 = 0,
    RTC_FAST_FREQ_8M     = 1
} rtc_fast_freq_t;
