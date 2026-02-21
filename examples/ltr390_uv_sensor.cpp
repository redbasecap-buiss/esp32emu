#include <Arduino.h>
#include <Adafruit_LTR390.h>

Adafruit_LTR390 ltr;

void setup() {
    Serial.begin(115200);
    Serial.println("LTR390 UV/Ambient Light Sensor");

    if (!ltr.begin()) {
        Serial.println("Failed to find LTR390!");
        while (1) delay(10);
    }
    Serial.println("LTR390 found!");

    ltr.setMode(LTR390_MODE_UVS);
    ltr.setGain(LTR390_GAIN_3);
    ltr.setResolution(LTR390_RESOLUTION_18BIT);
    ltr.enable(true);
}

void loop() {
    if (ltr.newDataAvailable()) {
        // Read UV
        ltr.setMode(LTR390_MODE_UVS);
        Serial.print("UV Index: ");
        Serial.println(ltr.getUVI(), 2);
        Serial.print("UV raw: ");
        Serial.println(ltr.readUVS());

        // Read ambient light
        ltr.setMode(LTR390_MODE_ALS);
        Serial.print("Lux: ");
        Serial.println(ltr.getLux(), 2);
        Serial.print("ALS raw: ");
        Serial.println(ltr.readALS());

        Serial.println("---");
    }
    delay(1000);
}
