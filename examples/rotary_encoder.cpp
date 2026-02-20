// Rotary encoder example using Encoder library
#include <Arduino.h>
#include <Encoder.h>

Encoder knob(2, 3);
long lastPosition = -999;

void setup() {
    Serial.begin(115200);
    Serial.println("Rotary Encoder Test");
}

void loop() {
    long pos = knob.read();
    if (pos != lastPosition) {
        Serial.print("Position: ");
        Serial.println(pos);
        lastPosition = pos;
    }
    delay(50);
}
