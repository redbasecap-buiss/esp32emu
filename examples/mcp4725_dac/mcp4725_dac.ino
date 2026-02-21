#include <Wire.h>
#include <MCP4725.h>

MCP4725 dac;

void setup() {
    Serial.begin(115200);
    if (!dac.begin(0x60)) {
        Serial.println("MCP4725 not found!");
        while (1) delay(10);
    }
    Serial.println("MCP4725 12-bit DAC ready");
}

void loop() {
    // Triangle wave: ramp up then down
    for (uint16_t v = 0; v < 4096; v += 64) {
        dac.setVoltage(v, false);
        Serial.print("DAC: "); Serial.print(v);
        Serial.print("  Voltage: "); Serial.print(dac.getVoltageFloat(3.3f), 3);
        Serial.println(" V");
        delay(50);
    }
    for (int v = 4095; v >= 0; v -= 64) {
        dac.setVoltage((uint16_t)v, false);
        delay(50);
    }
}
