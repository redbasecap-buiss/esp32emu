#include "esp_partition.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    esp32emu::partition::reset();

    // Find factory partition
    auto* factory = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, nullptr);
    assert(factory != nullptr);
    assert(strcmp(factory->label, "factory") == 0);
    assert(factory->type == ESP_PARTITION_TYPE_APP);

    // Find by label
    auto* nvs = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "nvs");
    assert(nvs != nullptr);
    assert(strcmp(nvs->label, "nvs") == 0);

    // Find OTA partitions
    auto* ota0 = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, nullptr);
    assert(ota0 != nullptr);
    assert(strcmp(ota0->label, "ota_0") == 0);

    // Not found
    auto* nope = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, "nonexistent");
    assert(nope == nullptr);

    // Read/Write
    const char* msg = "Hello Partition!";
    esp_err_t err = esp_partition_write(nvs, 0, msg, strlen(msg) + 1);
    assert(err == ESP_OK);

    char buf[64] = {};
    err = esp_partition_read(nvs, 0, buf, strlen(msg) + 1);
    assert(err == ESP_OK);
    assert(strcmp(buf, msg) == 0);

    // Erase
    err = esp_partition_erase_range(nvs, 0, 64);
    assert(err == ESP_OK);
    err = esp_partition_read(nvs, 0, buf, 4);
    assert(err == ESP_OK);
    assert((uint8_t)buf[0] == 0xFF);

    // Out of bounds
    err = esp_partition_write(nvs, nvs->size, msg, 1);
    assert(err == ESP_ERR_INVALID_SIZE);

    printf("test_partition: ALL PASSED\n");
    return 0;
}
