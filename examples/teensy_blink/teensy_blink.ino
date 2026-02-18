// Teensy 4.0 Blink Example
// Demonstrates basic GPIO on Teensy 4.0 (IMXRT1062 @600MHz)
//
// Build: make build/examples/teensy_blink
// Run:   ./build/examples/teensy_blink --board teensy

#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Teensy 4.0 Blink Example");
    Serial.println("IMXRT1062 running at 600MHz");

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);

    // Use analog read
    Serial.print("A0 value: ");
    Serial.println(analogRead(A0));
}

void loop() {
    // Chase pattern across 3 LEDs
    for (int pin : {LED_BUILTIN, 2, 3}) {
        digitalWrite(pin, HIGH);
        delay(200);
        digitalWrite(pin, LOW);
    }
    Serial.println("Tick");
}
