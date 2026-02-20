// RTC IO example – configure RTC GPIO for deep sleep wakeup
#include <Arduino.h>
#include <driver/rtc_io.h>
#include <esp_sleep.h>

#define WAKEUP_PIN 4  // RTC-capable GPIO

void setup() {
    Serial.begin(115200);

    rtc_gpio_init((gpio_num_t)WAKEUP_PIN);
    rtc_gpio_set_direction((gpio_num_t)WAKEUP_PIN, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_en((gpio_num_t)WAKEUP_PIN);
    rtc_gpio_pullup_dis((gpio_num_t)WAKEUP_PIN);

    Serial.println("RTC GPIO configured for wakeup");
    Serial.print("Pin level: ");
    Serial.println(rtc_gpio_get_level((gpio_num_t)WAKEUP_PIN));

    // Hold state during deep sleep
    rtc_gpio_hold_en((gpio_num_t)WAKEUP_PIN);

    Serial.println("Entering deep sleep...");
    esp_deep_sleep(10 * 1000000);  // 10 seconds
}

void loop() {}
