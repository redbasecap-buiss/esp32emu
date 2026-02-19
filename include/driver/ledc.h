#pragma once
// esp32emu — ESP-IDF LEDC driver mock
// Provides the low-level ledc_timer_config / ledc_channel_config API.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include "esp_err.h"

typedef enum { LEDC_HIGH_SPEED_MODE = 0, LEDC_LOW_SPEED_MODE, LEDC_SPEED_MODE_MAX } ledc_mode_t;
typedef enum {
    LEDC_TIMER_0 = 0, LEDC_TIMER_1, LEDC_TIMER_2, LEDC_TIMER_3, LEDC_TIMER_MAX
} ledc_timer_t;
typedef enum {
    LEDC_CHANNEL_0 = 0, LEDC_CHANNEL_1, LEDC_CHANNEL_2, LEDC_CHANNEL_3,
    LEDC_CHANNEL_4, LEDC_CHANNEL_5, LEDC_CHANNEL_6, LEDC_CHANNEL_7,
    LEDC_CHANNEL_MAX
} ledc_channel_t;
typedef enum {
    LEDC_TIMER_1_BIT = 1, LEDC_TIMER_2_BIT, LEDC_TIMER_3_BIT, LEDC_TIMER_4_BIT,
    LEDC_TIMER_5_BIT, LEDC_TIMER_6_BIT, LEDC_TIMER_7_BIT, LEDC_TIMER_8_BIT,
    LEDC_TIMER_9_BIT, LEDC_TIMER_10_BIT, LEDC_TIMER_11_BIT, LEDC_TIMER_12_BIT,
    LEDC_TIMER_13_BIT, LEDC_TIMER_14_BIT, LEDC_TIMER_15_BIT, LEDC_TIMER_16_BIT,
    LEDC_TIMER_17_BIT, LEDC_TIMER_18_BIT, LEDC_TIMER_19_BIT, LEDC_TIMER_20_BIT,
    LEDC_TIMER_BIT_MAX
} ledc_timer_bit_t;
typedef enum { LEDC_FADE_NO_WAIT = 0, LEDC_FADE_WAIT_DONE } ledc_fade_mode_t;

typedef struct {
    ledc_mode_t speed_mode;
    ledc_timer_bit_t duty_resolution;
    ledc_timer_t timer_num;
    uint32_t freq_hz;
    // clk_cfg omitted for simplicity
} ledc_timer_config_t;

typedef struct {
    int gpio_num;
    ledc_mode_t speed_mode;
    ledc_channel_t channel;
    // intr_type omitted
    ledc_timer_t timer_sel;
    uint32_t duty;
    int hpoint;
} ledc_channel_config_t;

namespace esp32emu {
struct LedcTimerState {
    uint32_t freq_hz = 0;
    ledc_timer_bit_t duty_resolution = LEDC_TIMER_13_BIT;
    bool configured = false;
};
struct LedcChannelState {
    int gpio_num = -1;
    ledc_timer_t timer_sel = LEDC_TIMER_0;
    uint32_t duty = 0;
    bool configured = false;
};
struct LedcState {
    LedcTimerState timers[LEDC_SPEED_MODE_MAX][LEDC_TIMER_MAX];
    LedcChannelState channels[LEDC_SPEED_MODE_MAX][LEDC_CHANNEL_MAX];
    bool fade_installed = false;
    void reset() { *this = LedcState{}; }
};
inline LedcState& ledc_state() { static LedcState s; return s; }
} // namespace esp32emu

inline esp_err_t ledc_timer_config(const ledc_timer_config_t* cfg) {
    if (!cfg || cfg->speed_mode >= LEDC_SPEED_MODE_MAX || cfg->timer_num >= LEDC_TIMER_MAX)
        return ESP_ERR_INVALID_ARG;
    auto& t = esp32emu::ledc_state().timers[cfg->speed_mode][cfg->timer_num];
    t.freq_hz = cfg->freq_hz;
    t.duty_resolution = cfg->duty_resolution;
    t.configured = true;
    fprintf(stderr, "[esp32emu] LEDC timer %d configured: %u Hz, %d-bit\n",
            cfg->timer_num, cfg->freq_hz, (int)cfg->duty_resolution);
    return ESP_OK;
}

inline esp_err_t ledc_channel_config(const ledc_channel_config_t* cfg) {
    if (!cfg || cfg->speed_mode >= LEDC_SPEED_MODE_MAX || cfg->channel >= LEDC_CHANNEL_MAX)
        return ESP_ERR_INVALID_ARG;
    auto& ch = esp32emu::ledc_state().channels[cfg->speed_mode][cfg->channel];
    ch.gpio_num = cfg->gpio_num;
    ch.timer_sel = cfg->timer_sel;
    ch.duty = cfg->duty;
    ch.configured = true;
    fprintf(stderr, "[esp32emu] LEDC channel %d: GPIO %d, duty %u\n",
            cfg->channel, cfg->gpio_num, cfg->duty);
    return ESP_OK;
}

inline esp_err_t ledc_set_duty(ledc_mode_t mode, ledc_channel_t ch, uint32_t duty) {
    if (mode >= LEDC_SPEED_MODE_MAX || ch >= LEDC_CHANNEL_MAX) return ESP_ERR_INVALID_ARG;
    esp32emu::ledc_state().channels[mode][ch].duty = duty;
    return ESP_OK;
}

inline esp_err_t ledc_update_duty(ledc_mode_t mode, ledc_channel_t ch) {
    if (mode >= LEDC_SPEED_MODE_MAX || ch >= LEDC_CHANNEL_MAX) return ESP_ERR_INVALID_ARG;
    auto& c = esp32emu::ledc_state().channels[mode][ch];
    fprintf(stderr, "[esp32emu] LEDC channel %d duty updated: %u\n", ch, c.duty);
    return ESP_OK;
}

inline uint32_t ledc_get_duty(ledc_mode_t mode, ledc_channel_t ch) {
    if (mode >= LEDC_SPEED_MODE_MAX || ch >= LEDC_CHANNEL_MAX) return 0;
    return esp32emu::ledc_state().channels[mode][ch].duty;
}

inline esp_err_t ledc_set_freq(ledc_mode_t mode, ledc_timer_t timer, uint32_t freq) {
    if (mode >= LEDC_SPEED_MODE_MAX || timer >= LEDC_TIMER_MAX) return ESP_ERR_INVALID_ARG;
    esp32emu::ledc_state().timers[mode][timer].freq_hz = freq;
    return ESP_OK;
}

inline uint32_t ledc_get_freq(ledc_mode_t mode, ledc_timer_t timer) {
    if (mode >= LEDC_SPEED_MODE_MAX || timer >= LEDC_TIMER_MAX) return 0;
    return esp32emu::ledc_state().timers[mode][timer].freq_hz;
}

inline esp_err_t ledc_fade_func_install(int flags) {
    (void)flags;
    esp32emu::ledc_state().fade_installed = true;
    return ESP_OK;
}

inline esp_err_t ledc_set_fade_with_time(ledc_mode_t mode, ledc_channel_t ch,
                                          uint32_t target_duty, int max_fade_time_ms) {
    (void)max_fade_time_ms;
    if (mode >= LEDC_SPEED_MODE_MAX || ch >= LEDC_CHANNEL_MAX) return ESP_ERR_INVALID_ARG;
    esp32emu::ledc_state().channels[mode][ch].duty = target_duty;
    return ESP_OK;
}

inline esp_err_t ledc_fade_start(ledc_mode_t mode, ledc_channel_t ch, ledc_fade_mode_t fade_mode) {
    (void)fade_mode;
    if (mode >= LEDC_SPEED_MODE_MAX || ch >= LEDC_CHANNEL_MAX) return ESP_ERR_INVALID_ARG;
    auto& c = esp32emu::ledc_state().channels[mode][ch];
    fprintf(stderr, "[esp32emu] LEDC fade channel %d → duty %u\n", ch, c.duty);
    return ESP_OK;
}

inline esp_err_t ledc_stop(ledc_mode_t mode, ledc_channel_t ch, uint32_t idle_level) {
    if (mode >= LEDC_SPEED_MODE_MAX || ch >= LEDC_CHANNEL_MAX) return ESP_ERR_INVALID_ARG;
    esp32emu::ledc_state().channels[mode][ch].duty = idle_level;
    fprintf(stderr, "[esp32emu] LEDC channel %d stopped, idle=%u\n", ch, idle_level);
    return ESP_OK;
}
