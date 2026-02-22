// ESP32Encoder — rotary encoder position tracking
#include <Arduino.h>
#include <ESP32Encoder.h>

ESP32Encoder encoder;

void setup() {
    Serial.begin(115200);
    ESP32Encoder::useInternalWeakPullResistors(ESP32Encoder::UP);
    encoder.attachFullQuad(25, 26);
    encoder.setCount(0);
    Serial.println("Rotary encoder ready. Monitoring position...");
}

void loop() {
    int64_t count = encoder.getCount();
    float angle = count * 360.0f / 600.0f;  // 600 PPR encoder
    Serial.printf("Count: %lld  Angle: %.1f°\n", (long long)count, angle);
    delay(100);
}
