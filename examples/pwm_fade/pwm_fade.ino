// PWM LED fade using LEDC controller
// Demonstrates ledcSetup, ledcAttachPin, ledcWrite

#include <Arduino.h>

const int LED_PIN = 2;
const int LEDC_CHANNEL = 0;
const int LEDC_FREQ = 5000;
const int LEDC_RESOLUTION = 8;

void setup() {
    Serial.begin(115200);
    Serial.println("PWM LED Fade Demo");

    ledcSetup(LEDC_CHANNEL, LEDC_FREQ, LEDC_RESOLUTION);
    ledcAttachPin(LED_PIN, LEDC_CHANNEL);
}

void loop() {
    // Fade up
    for (int duty = 0; duty <= 255; duty += 5) {
        ledcWrite(LEDC_CHANNEL, duty);
        Serial.print("Brightness: ");
        Serial.print((duty * 100) / 255);
        Serial.println("%");
        delay(30);
    }
    // Fade down
    for (int duty = 255; duty >= 0; duty -= 5) {
        ledcWrite(LEDC_CHANNEL, duty);
        delay(30);
    }
}
