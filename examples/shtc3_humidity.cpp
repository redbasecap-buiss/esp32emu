// SHTC3 humidity/temperature sensor — read temp and humidity with sleep mode
#include <Arduino.h>
#include <Wire.h>
#include <SHTC3.h>

SHTC3 mySHTC3;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    if (mySHTC3.begin() != SHTC3_Status_Nominal) {
        Serial.println("SHTC3 not found!");
        while (1) delay(100);
    }
    Serial.println("SHTC3 initialized");
}

void loop() {
    mySHTC3.wake();
    mySHTC3.update();

    Serial.print("Temperature: ");
    Serial.print(mySHTC3.toDegC());
    Serial.print(" °C (");
    Serial.print(mySHTC3.toDegF());
    Serial.println(" °F)");

    Serial.print("Humidity: ");
    Serial.print(mySHTC3.toPercent());
    Serial.println(" %RH");
    Serial.println("---");

    mySHTC3.sleep(); // Low power between readings
    delay(2000);
}
