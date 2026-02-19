// Sigma-Delta LED dimming example
// Uses sigma-delta modulation to smoothly dim an LED.

#include <Arduino.h>
#include "driver/sigmadelta.h"

#define LED_GPIO 18

void setup() {
    Serial.begin(115200);
    Serial.println("Sigma-Delta LED Dimmer");

    sigmadelta_config_t config = {};
    config.channel = SIGMADELTA_CHANNEL_0;
    config.sigmadelta_duty = 0;
    config.sigmadelta_prescale = 80;
    config.sigmadelta_gpio = LED_GPIO;
    sigmadelta_config(&config);
}

void loop() {
    // Fade up
    Serial.println("💡 Fading up...");
    for (int duty = -128; duty <= 127; duty += 8) {
        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, (int8_t)duty);
        Serial.printf("  duty: %d\n", duty);
        delay(50);
    }

    // Fade down
    Serial.println("💡 Fading down...");
    for (int duty = 127; duty >= -128; duty -= 8) {
        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, (int8_t)duty);
        Serial.printf("  duty: %d\n", duty);
        delay(50);
    }

    delay(500);
}
