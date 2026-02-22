// Modbus RTU — Read energy meter (voltage, current, power)
#include <Arduino.h>
#include <ModbusMaster.h>

ModbusMaster meter;

void setup() {
    Serial.begin(115200);
    Serial2.begin(9600);
    meter.begin(1, Serial2);

    // Simulate meter registers: voltage=230V, current=5A, power=1150W
    meter.emu_setRegister(0x0000, 2300);  // voltage * 10
    meter.emu_setRegister(0x0001, 50);    // current * 10
    meter.emu_setRegister(0x0002, 1150);  // power in W

    Serial.println("Energy Meter Monitor Ready");
}

void loop() {
    uint8_t result = meter.readHoldingRegisters(0x0000, 3);
    if (result == ModbusMaster::ku8MBSuccess) {
        float voltage = meter.getResponseBuffer(0) / 10.0f;
        float current = meter.getResponseBuffer(1) / 10.0f;
        uint16_t power = meter.getResponseBuffer(2);
        Serial.printf("Voltage: %.1f V | Current: %.1f A | Power: %u W\n",
                       voltage, current, power);
    } else {
        Serial.println("Read failed!");
    }

    // Write max power threshold
    meter.writeSingleRegister(0x0010, 3000);  // 3000W max
    delay(5000);
}
