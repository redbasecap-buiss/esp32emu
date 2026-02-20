// esp32emu example: DallasTemperature Multi-Sensor
// Demonstrates reading multiple DS18B20 temperature sensors on one bus.

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
    Serial.begin(115200);
    Serial.println("Dallas Temperature Multi-Sensor Demo");

    // Add test devices (in real hardware, these are auto-discovered)
    uint8_t indoor[8]  = {0x28, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t outdoor[8] = {0x28, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
    oneWire.addDevice(indoor);
    oneWire.addDevice(outdoor);

    sensors.begin();
    sensors.setResolution(12);

    Serial.printf("Found %d sensors\n", sensors.getDeviceCount());

    // Inject test temperatures
    sensors.setTemperatureByIndex(0, 21.5f);  // Indoor
    sensors.setTemperatureByIndex(1, -3.2f);  // Outdoor
}

void loop() {
    sensors.requestTemperatures();

    for (uint8_t i = 0; i < sensors.getDeviceCount(); i++) {
        DeviceAddress addr;
        if (sensors.getAddress(addr, i)) {
            float tempC = sensors.getTempCByIndex(i);
            float tempF = sensors.getTempFByIndex(i);
            Serial.printf("Sensor %d: %.1f°C (%.1f°F)\n", i, tempC, tempF);
        }
    }
    Serial.println();

    delay(2000);
}
