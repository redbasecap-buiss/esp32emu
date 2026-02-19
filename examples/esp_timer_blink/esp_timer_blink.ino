// High-resolution timer blink using esp_timer
// Demonstrates esp_timer_create, start_periodic, stop

#include <Arduino.h>
#include <esp_timer.h>
#include <esp_log.h>

static const char* TAG = "timer_blink";
static bool led_state = false;

void blink_callback(void* arg) {
    (void)arg;
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state ? HIGH : LOW);
    ESP_LOGI(TAG, "LED %s (timer fired at %lld us)",
             led_state ? "ON" : "OFF", esp_timer_get_time());
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(TAG, "Starting high-resolution timer blink");

    esp_timer_create_args_t args = {};
    args.callback = blink_callback;
    args.name = "blink_timer";

    esp_timer_handle_t timer;
    esp_timer_create(&args, &timer);
    esp_timer_start_periodic(timer, 500000); // 500ms = 0.5s blink

    ESP_LOGI(TAG, "Timer started, blinking every 500ms");
}

void loop() {
    delay(5000);
    ESP_LOGI(TAG, "Still running... uptime: %lld ms", esp_timer_get_time() / 1000);
}
