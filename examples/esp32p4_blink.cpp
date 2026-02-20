// ESP32-P4 Blink Example
// Demonstrates using the ESP32-P4 board variant (400MHz RISC-V)
// Run with: ./esp32emu run --board esp32p4 examples/esp32p4_blink.cpp
#include <Arduino.h>

#define LED_PIN 2

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    Serial.println("ESP32-P4 Blink Example");
    Serial.println("400MHz RISC-V with WiFi/BT, MIPI-DSI/CSI, USB-OTG");
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    delay(1000);

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    delay(1000);
}
