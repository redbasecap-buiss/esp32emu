// ESP-IDF partition read/write example
#include "Arduino.h"
#include "esp_partition.h"

void setup() {
    Serial.begin(115200);
    Serial.println("=== Partition Read/Write Example ===");

    // Find NVS partition
    const esp_partition_t* nvs = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
    if (!nvs) {
        Serial.println("NVS partition not found!");
        return;
    }
    Serial.print("Found partition: "); Serial.println(nvs->label);
    Serial.print("Size: "); Serial.print(nvs->size); Serial.println(" bytes");

    // Write data
    const char* data = "Hello from partition API!";
    esp_err_t err = esp_partition_write(nvs, 0, data, strlen(data) + 1);
    Serial.print("Write: "); Serial.println(err == ESP_OK ? "OK" : "FAIL");

    // Read back
    char buf[64] = {};
    err = esp_partition_read(nvs, 0, buf, strlen(data) + 1);
    Serial.print("Read: "); Serial.println(buf);

    // Erase and verify
    esp_partition_erase_range(nvs, 0, 4096);
    esp_partition_read(nvs, 0, buf, 4);
    Serial.print("After erase, first byte: 0x");
    Serial.println((uint8_t)buf[0], HEX);

    // List all app partitions
    Serial.println("\nApp partitions:");
    for (auto sub : {ESP_PARTITION_SUBTYPE_APP_FACTORY, ESP_PARTITION_SUBTYPE_APP_OTA_0, ESP_PARTITION_SUBTYPE_APP_OTA_1}) {
        auto* p = esp_partition_find_first(ESP_PARTITION_TYPE_APP, sub, nullptr);
        if (p) {
            Serial.print("  "); Serial.print(p->label);
            Serial.print(" @ 0x"); Serial.print(p->address, HEX);
            Serial.print(" size=0x"); Serial.println(p->size, HEX);
        }
    }
}

void loop() {
    delay(5000);
}
