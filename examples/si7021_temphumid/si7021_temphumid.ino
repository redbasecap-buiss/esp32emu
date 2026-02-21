#include <Wire.h>
#include <Adafruit_SI7021.h>

Adafruit_SI7021 sensor;

void setup() {
    Serial.begin(115200);
    if (!sensor.begin()) {
        Serial.println("SI7021 not found!");
        while (1) delay(10);
    }
    Serial.print("SI7021 Rev: 0x"); Serial.println(sensor.getRevision(), HEX);
    Serial.println("SI7021 Temperature & Humidity Sensor ready");
}

void loop() {
    float temp = sensor.readTemperature();
    float hum = sensor.readHumidity();
    Serial.print("Temperature: "); Serial.print(temp, 1); Serial.print(" °C  ");
    Serial.print("Humidity: "); Serial.print(hum, 1); Serial.println(" %");
    delay(2000);
}
