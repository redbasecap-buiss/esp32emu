// ESP-IDF OTA operations example
// Demonstrates partition-level OTA using esp_ota_ops API

#include "esp_ota_ops.h"
#include "esp_system.h"

void setup() {
    Serial.begin(115200);
    Serial.println("=== ESP-IDF OTA Example ===");

    esp_chip_info_t ci;
    esp_chip_info(&ci);
    Serial.print("Chip cores: "); Serial.println(ci.cores);
    Serial.print("Free heap: "); Serial.println(esp_get_free_heap_size());

    const esp_partition_t* running = esp_ota_get_running_partition();
    Serial.print("Running from: "); Serial.println(running->label);

    const esp_partition_t* update = esp_ota_get_next_update_partition(nullptr);
    Serial.print("Update target: "); Serial.println(update->label);

    esp_ota_handle_t handle;
    if (esp_ota_begin(update, 0, &handle) == ESP_OK) {
        const char* fw = "FIRMWARE_V2";
        esp_ota_write(handle, fw, strlen(fw));
        esp_ota_end(handle);
        esp_ota_set_boot_partition(update);
        Serial.println("OTA complete!");
    }
}

void loop() { delay(1000); }
