// esp32emu test — ESP-IDF LEDC driver
#include "driver/ledc.h"
#include <cassert>
#include <cstdio>

int main() {
    esp32emu::ledc_state().reset();

    // Configure timer
    ledc_timer_config_t timer_cfg = {};
    timer_cfg.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_cfg.timer_num = LEDC_TIMER_0;
    timer_cfg.duty_resolution = LEDC_TIMER_13_BIT;
    timer_cfg.freq_hz = 5000;
    assert(ledc_timer_config(&timer_cfg) == ESP_OK);

    auto& ts = esp32emu::ledc_state().timers[LEDC_HIGH_SPEED_MODE][LEDC_TIMER_0];
    assert(ts.configured);
    assert(ts.freq_hz == 5000);

    // Configure channel
    ledc_channel_config_t ch_cfg = {};
    ch_cfg.speed_mode = LEDC_HIGH_SPEED_MODE;
    ch_cfg.channel = LEDC_CHANNEL_0;
    ch_cfg.timer_sel = LEDC_TIMER_0;
    ch_cfg.gpio_num = 18;
    ch_cfg.duty = 4096;
    ch_cfg.hpoint = 0;
    assert(ledc_channel_config(&ch_cfg) == ESP_OK);

    auto& cs = esp32emu::ledc_state().channels[LEDC_HIGH_SPEED_MODE][LEDC_CHANNEL_0];
    assert(cs.configured);
    assert(cs.gpio_num == 18);
    assert(cs.duty == 4096);

    // Set and get duty
    assert(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 2048) == ESP_OK);
    assert(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0) == 2048);
    assert(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0) == ESP_OK);

    // Set and get frequency
    assert(ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, 10000) == ESP_OK);
    assert(ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0) == 10000);

    // Fade
    assert(ledc_fade_func_install(0) == ESP_OK);
    assert(esp32emu::ledc_state().fade_installed);
    assert(ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 8000, 1000) == ESP_OK);
    assert(ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE) == ESP_OK);
    assert(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0) == 8000);

    // Stop
    assert(ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0) == ESP_OK);
    assert(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0) == 0);

    // Invalid args
    assert(ledc_timer_config(nullptr) == ESP_ERR_INVALID_ARG);
    assert(ledc_channel_config(nullptr) == ESP_ERR_INVALID_ARG);

    printf("test_ledc_driver: all assertions passed\n");
    return 0;
}
