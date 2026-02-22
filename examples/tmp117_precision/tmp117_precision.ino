// TMP117 High-Precision Temperature Sensor
// Reads temperature with ±0.1°C accuracy and alert monitoring.

#include <Arduino.h>
#include <Wire.h>
#include <TMP117.h>

TMP117 sensor;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!sensor.begin()) {
        Serial.println("TMP117 not found!");
        while (1) delay(100);
    }

    Serial.print("Device ID: 0x");
    Serial.println(sensor.getDeviceID(), HEX);

    sensor.setHighLimit(35.0);
    sensor.setLowLimit(15.0);
    sensor.setAveraging(3);  // 64x averaging for precision

    Serial.println("TMP117 ready — high-precision temperature monitoring");
}

void loop() {
    if (sensor.dataReady()) {
        float tempC = sensor.readTempC();
        float tempF = sensor.readTempF();

        Serial.print("Temperature: ");
        Serial.print(tempC, 4);
        Serial.print(" °C  (");
        Serial.print(tempF, 2);
        Serial.println(" °F)");

        if (sensor.getHighAlert()) {
            Serial.println("⚠️  HIGH TEMPERATURE ALERT!");
        }
        if (sensor.getLowAlert()) {
            Serial.println("⚠️  LOW TEMPERATURE ALERT!");
        }
    }

    delay(1000);
}
