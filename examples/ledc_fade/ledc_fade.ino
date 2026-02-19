// ESP-IDF LEDC fade example — smooth LED brightness control
#include <Arduino.h>
#include "driver/ledc.h"

#define LED_GPIO    2
#define LEDC_FREQ   5000
#define LEDC_RES    LEDC_TIMER_13_BIT
#define MAX_DUTY    ((1 << 13) - 1)  // 8191

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF LEDC Fade Example");

    // Configure timer
    ledc_timer_config_t timer_cfg = {};
    timer_cfg.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_cfg.timer_num = LEDC_TIMER_0;
    timer_cfg.duty_resolution = LEDC_RES;
    timer_cfg.freq_hz = LEDC_FREQ;
    ledc_timer_config(&timer_cfg);

    // Configure channel
    ledc_channel_config_t ch_cfg = {};
    ch_cfg.speed_mode = LEDC_HIGH_SPEED_MODE;
    ch_cfg.channel = LEDC_CHANNEL_0;
    ch_cfg.timer_sel = LEDC_TIMER_0;
    ch_cfg.gpio_num = LED_GPIO;
    ch_cfg.duty = 0;
    ch_cfg.hpoint = 0;
    ledc_channel_config(&ch_cfg);

    // Install fade service
    ledc_fade_func_install(0);
    Serial.println("LEDC configured, starting fade...");
}

void loop() {
    // Fade up
    Serial.println("Fading up...");
    ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, MAX_DUTY, 2000);
    ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE);

    Serial.print("Duty: ");
    Serial.println(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0));
    delay(500);

    // Fade down
    Serial.println("Fading down...");
    ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0, 2000);
    ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE);

    Serial.print("Duty: ");
    Serial.println(ledc_get_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0));
    delay(500);
}
