#include <Wire.h>
#include <Adafruit_VEML7700.h>

Adafruit_VEML7700 veml;

void setup() {
    Serial.begin(115200);
    if (!veml.begin()) {
        Serial.println("VEML7700 not found!");
        while (1) delay(10);
    }
    veml.setGain(Adafruit_VEML7700::VEML7700_GAIN_1);
    veml.setIntegrationTime(Adafruit_VEML7700::VEML7700_IT_100MS);
    Serial.println("VEML7700 Ambient Light Sensor ready");
}

void loop() {
    float lux = veml.readLux();
    uint16_t als = veml.readALS();
    uint16_t white = veml.readWhite();
    Serial.print("Lux: "); Serial.print(lux);
    Serial.print("  ALS: "); Serial.print(als);
    Serial.print("  White: "); Serial.println(white);
    delay(1000);
}
