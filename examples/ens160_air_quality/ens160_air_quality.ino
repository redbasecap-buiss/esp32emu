// ENS160 Air Quality Monitor
// Reads TVOC, eCO2, and AQI index for indoor air quality.

#include <Arduino.h>
#include <Wire.h>
#include <ENS160.h>

ENS160 ens;

const char* aqiLabel(uint8_t aqi) {
    switch (aqi) {
        case 1: return "Excellent";
        case 2: return "Good";
        case 3: return "Moderate";
        case 4: return "Poor";
        case 5: return "Unhealthy";
        default: return "Unknown";
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!ens.begin()) {
        Serial.println("ENS160 not found!");
        while (1) delay(100);
    }

    ens.setMode(ENS160::MODE_STANDARD);
    ens.setTempCompensation(22.0);
    ens.setRHCompensation(50.0);

    Serial.println("ENS160 Air Quality Monitor");
    Serial.println("==========================");
}

void loop() {
    if (ens.available()) {
        uint8_t aqi = ens.getAQI();
        uint16_t tvoc = ens.getTVOC();
        uint16_t eco2 = ens.geteCO2();

        Serial.print("AQI: ");
        Serial.print(aqi);
        Serial.print(" (");
        Serial.print(aqiLabel(aqi));
        Serial.print(")  TVOC: ");
        Serial.print(tvoc);
        Serial.print(" ppb  eCO2: ");
        Serial.print(eco2);
        Serial.println(" ppm");

        if (aqi >= 4) {
            Serial.println("⚠️  Open a window! Air quality is poor.");
        }
    }

    delay(2000);
}
