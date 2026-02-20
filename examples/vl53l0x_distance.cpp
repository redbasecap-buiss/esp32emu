// VL53L0X Time-of-Flight distance sensor example
#include "Arduino.h"
#include "VL53L0X.h"
#include <cstdio>

VL53L0X sensor;

void setup() {
    Serial.begin(115200);
    if (!sensor.begin()) {
        Serial.println("VL53L0X not found!");
        while (1) {}
    }
    sensor.setTimeout(500);
    sensor.setMeasurementTimingBudget(200000); // high accuracy mode
    Serial.println("VL53L0X ready");

    // Inject test values
    sensor.setDistance(150);
}

void loop() {
    uint16_t distance = sensor.readRangeSingleMillimeters();
    if (sensor.timeoutOccurred()) {
        Serial.println("TIMEOUT");
    } else {
        printf("Distance: %d mm\n", distance);
    }
    delay(100);
}
