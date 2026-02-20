// SoftwareSerial GPS Reader Example
// Reads NMEA sentences from a GPS module on pins 4/5
#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial gpsSerial(4, 5); // RX, TX

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600);
    Serial.println("GPS Reader started");
}

void loop() {
    while (gpsSerial.available()) {
        char c = gpsSerial.read();
        Serial.print(c);
    }
    delay(10);
}
