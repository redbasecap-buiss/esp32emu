// CAP1188 Capacitive Touch Buttons
// 8-channel touch input with LED feedback.

#include <Arduino.h>
#include <Wire.h>
#include <CAP1188.h>

CAP1188 cap;

const char* channelNames[] = {
    "Play", "Stop", "Next", "Prev",
    "Vol+", "Vol-", "Mute", "Menu"
};

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!cap.begin()) {
        Serial.println("CAP1188 not found!");
        while (1) delay(100);
    }

    cap.setSensitivity(5);
    cap.enableMultiTouch(true, 2);

    Serial.println("CAP1188 Touch Panel Ready");
    Serial.println("Touch any of 8 capacitive pads:");
    for (int i = 0; i < 8; i++) {
        Serial.print("  [");
        Serial.print(i);
        Serial.print("] ");
        Serial.println(channelNames[i]);
    }
}

void loop() {
    uint8_t touched = cap.touched();

    if (touched) {
        for (uint8_t i = 0; i < 8; i++) {
            if (touched & (1 << i)) {
                Serial.print("🔘 Touched: ");
                Serial.print(channelNames[i]);
                Serial.print(" (channel ");
                Serial.print(i);
                Serial.println(")");
            }
        }
    }

    delay(50);
}
