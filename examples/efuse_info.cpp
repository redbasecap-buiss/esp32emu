// ESP32 eFuse Information Example
// Reads chip revision and eFuse data
#include <Arduino.h>
#include "esp_efuse.h"

void setup() {
    Serial.begin(115200);

    Serial.println("=== eFuse Information ===");
    Serial.printf("Chip revision: %u.%u\n",
        esp_efuse_get_major_chip_version(),
        esp_efuse_get_minor_chip_version());

    // Read first 8 bytes of eFuse block 0
    uint8_t data[8] = {};
    if (esp_efuse_read_block(ESP_EFUSE_BLOCK0, data, 0, 64) == ESP_OK) {
        Serial.print("Block0[0:7]: ");
        for (int i = 0; i < 8; i++) {
            Serial.printf("%02X ", data[i]);
        }
        Serial.println();
    }
}

void loop() {
    delay(10000);
}
