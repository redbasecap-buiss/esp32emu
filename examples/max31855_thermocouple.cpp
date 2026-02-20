// MAX31855 Thermocouple reader example
#include "Arduino.h"
#include "MAX31855.h"
#include <cstdio>
#include <cmath>

#define CS_PIN 10

Adafruit_MAX31855 thermocouple(CS_PIN);

void setup() {
    Serial.begin(115200);
    if (!thermocouple.begin()) {
        Serial.println("MAX31855 not found!");
        while (1) {}
    }
    Serial.println("MAX31855 ready");
    thermocouple.setTemperature(180.0);  // simulating oven temp
}

void loop() {
    double c = thermocouple.readCelsius();
    if (std::isnan(c)) {
        uint8_t err = thermocouple.readError();
        if (err & Adafruit_MAX31855::FAULT_OPEN) Serial.println("FAULT: Open circuit");
        if (err & Adafruit_MAX31855::FAULT_SHORT_GND) Serial.println("FAULT: Short to GND");
        if (err & Adafruit_MAX31855::FAULT_SHORT_VCC) Serial.println("FAULT: Short to VCC");
    } else {
        double f = thermocouple.readFahrenheit();
        double internal = thermocouple.readInternal();
        printf("Thermocouple: %.1f°C / %.1f°F  (internal: %.1f°C)\n", c, f, internal);
    }
    delay(1000);
}
