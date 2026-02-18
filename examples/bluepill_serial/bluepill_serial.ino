// STM32 Blue Pill Serial Monitor Example
// Demonstrates SoftwareSerial + GPIO on STM32F103C8
//
// Build: make build/examples/bluepill_serial
// Run:   ./build/examples/bluepill_serial --board bluepill

#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial gpsSerial(10, 11); // RX, TX

void setup() {
    Serial.begin(115200);
    Serial.println("STM32 Blue Pill - Serial Bridge");
    Serial.println("STM32F103C8 @72MHz");

    gpsSerial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.println("Monitoring GPS on SoftwareSerial...");
}

void loop() {
    // Toggle LED to show activity
    digitalWrite(LED_BUILTIN, HIGH);

    // Read from software serial (GPS module)
    while (gpsSerial.available()) {
        char c = gpsSerial.read();
        Serial.print(c);
    }

    // Read from hardware serial (user commands)
    while (Serial.available()) {
        char c = Serial.read();
        gpsSerial.write(c);
    }

    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
}
