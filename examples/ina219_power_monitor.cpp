// esp32emu example: INA219 Current/Power Monitor
// Demonstrates reading bus voltage, current, and power from an INA219 sensor.

#include <Arduino.h>
#include <Wire.h>
#include <INA219.h>

INA219 ina219(0x40);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!ina219.begin()) {
        Serial.println("Failed to find INA219 sensor!");
        while (1) delay(10);
    }

    // Configure for 32V, 2A max
    ina219.setCalibration_32V_2A();

    Serial.println("INA219 Power Monitor");
    Serial.println("====================");

    // For testing: inject realistic solar panel readings
    ina219.setReadings(12.6f, 850.0f, 0.1f);
}

void loop() {
    float busVoltage = ina219.getBusVoltage_V();
    float shuntVoltage = ina219.getShuntVoltage_mV();
    float current = ina219.getCurrent_mA();
    float power = ina219.getPower_mW();

    Serial.println("---");
    Serial.printf("Bus Voltage:   %.2f V\n", busVoltage);
    Serial.printf("Shunt Voltage: %.2f mV\n", shuntVoltage);
    Serial.printf("Current:       %.2f mA\n", current);
    Serial.printf("Power:         %.2f mW\n", power);
    Serial.printf("Load Voltage:  %.2f V\n", busVoltage + (shuntVoltage / 1000.0f));

    delay(2000);
}
