// ESP-IDF system information example
#include "Arduino.h"
#include "esp_system.h"

void setup() {
    Serial.begin(115200);
    Serial.println("=== ESP32 System Info ===");

    // Chip info
    esp_chip_info_t ci;
    esp_chip_info(&ci);
    Serial.print("Model: ");
    switch (ci.model) {
        case ESP_CHIP_MODEL_ESP32:   Serial.println("ESP32"); break;
        case ESP_CHIP_MODEL_ESP32S2: Serial.println("ESP32-S2"); break;
        case ESP_CHIP_MODEL_ESP32S3: Serial.println("ESP32-S3"); break;
        case ESP_CHIP_MODEL_ESP32C3: Serial.println("ESP32-C3"); break;
        default: Serial.println("Unknown"); break;
    }
    Serial.print("Cores: "); Serial.println(ci.cores);
    Serial.print("Revision: "); Serial.println(ci.revision);

    Serial.print("Features:");
    if (ci.features & CHIP_FEATURE_WIFI_BGN) Serial.print(" WiFi");
    if (ci.features & CHIP_FEATURE_BLE)      Serial.print(" BLE");
    if (ci.features & CHIP_FEATURE_BT)       Serial.print(" BT");
    Serial.println();

    // Memory
    Serial.print("Free heap: "); Serial.print(esp_get_free_heap_size()); Serial.println(" bytes");
    Serial.print("Min free heap: "); Serial.print(esp_get_minimum_free_heap_size()); Serial.println(" bytes");

    // IDF version
    Serial.print("IDF version: "); Serial.println(esp_get_idf_version());

    // Reset reason
    Serial.print("Reset reason: ");
    switch (esp_reset_reason()) {
        case ESP_RST_POWERON:   Serial.println("Power-on"); break;
        case ESP_RST_SW:        Serial.println("Software"); break;
        case ESP_RST_PANIC:     Serial.println("Panic"); break;
        case ESP_RST_DEEPSLEEP: Serial.println("Deep sleep"); break;
        default: Serial.println("Other"); break;
    }

    // Random
    Serial.print("Random: "); Serial.println(esp_random());
}

void loop() {
    delay(5000);
}
