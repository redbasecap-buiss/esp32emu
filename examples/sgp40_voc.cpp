// esp32emu example — SGP40 VOC Index Sensor
#include "Arduino.h"
#include "Adafruit_SGP40.h"

Adafruit_SGP40 sgp;

void setup() {
    Serial.begin(115200);
    Serial.println("SGP40 VOC Sensor Demo");

    if (!sgp.begin()) {
        Serial.println("SGP40 not found!");
        while (1) delay(10);
    }

    // Simulate varying air quality
    sgp.test_setRawVOC(28000);
    sgp.test_setVocIndex(150);
}

void loop() {
    uint16_t raw = sgp.measureRaw(25.0, 50.0);
    int32_t voc = sgp.measureVocIndex(25.0, 50.0);

    Serial.print("Raw VOC: "); Serial.println(raw);
    Serial.print("VOC Index: "); Serial.println(voc);

    if (voc < 100) Serial.println("Air quality: Good");
    else if (voc < 200) Serial.println("Air quality: Moderate");
    else Serial.println("Air quality: Poor");

    delay(1000);
}
