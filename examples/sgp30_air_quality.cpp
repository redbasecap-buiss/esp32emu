// esp32emu example — SGP30 Air Quality Sensor (TVOC + eCO2)
#include <Arduino.h>
#include <SGP30.h>

SGP30 sgp;

void setup() {
    Serial.begin(115200);
    Serial.println("=== SGP30 Air Quality Demo ===");

    if (!sgp.begin()) {
        Serial.println("❌ SGP30 not found!");
        return;
    }

    uint16_t serial[3];
    sgp.getSerialNumber(serial);
    Serial.printf("Serial: %04X-%04X-%04X\n", serial[0], serial[1], serial[2]);

    // Restore baseline if available
    sgp.setIAQBaseline(0x8A12, 0x9134);
    Serial.println("✅ Baseline restored");
}

void loop() {
    if (sgp.IAQmeasure()) {
        Serial.printf("TVOC: %d ppb | eCO2: %d ppm\n", sgp.TVOC, sgp.eCO2);
    }

    if (sgp.IAQmeasureRaw()) {
        Serial.printf("Raw H2: %d | Raw Ethanol: %d\n", sgp.rawH2, sgp.rawEthanol);
    }

    // Save baseline periodically
    uint16_t eco2_base, tvoc_base;
    sgp.getIAQBaseline(&eco2_base, &tvoc_base);
    Serial.printf("Baseline eCO2: 0x%04X, TVOC: 0x%04X\n", eco2_base, tvoc_base);

    delay(1000);
}
