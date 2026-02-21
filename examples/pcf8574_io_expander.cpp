// esp32emu example — PCF8574 I2C I/O Expander
#include <Arduino.h>
#include <PCF8574.h>

PCF8574 expander(0x20);

void setup() {
    Serial.begin(115200);
    Serial.println("=== PCF8574 I/O Expander Demo ===");

    if (!expander.begin()) {
        Serial.println("❌ PCF8574 not found!");
        return;
    }
    Serial.printf("✅ PCF8574 found at 0x%02X\n", expander.getAddress());

    // Set all outputs LOW
    expander.write8(0x00);
    Serial.printf("Port: 0x%02X\n", expander.read8());

    // Turn on pins one by one
    for (uint8_t i = 0; i < 8; i++) {
        expander.write(i, HIGH);
        Serial.printf("Pin %d ON → Port: 0x%02X\n", i, expander.read8());
        delay(200);
    }

    // Toggle each pin
    Serial.println("\nToggling pins:");
    for (uint8_t i = 0; i < 8; i++) {
        expander.toggle(i);
        Serial.printf("Toggle pin %d → Port: 0x%02X\n", i, expander.read8());
    }

    // Read individual pins
    Serial.println("\nReading pins:");
    for (uint8_t i = 0; i < 8; i++) {
        Serial.printf("Pin %d = %d\n", i, expander.read(i));
    }
}

void loop() { delay(1000); }
