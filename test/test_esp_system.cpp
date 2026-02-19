#include "esp_system.h"
#include <cassert>
#include <cstdio>

int main() {
    // Reset reason
    esp32emu::system::reset();
    assert(esp_reset_reason() == ESP_RST_POWERON);
    esp32emu::system::_reset_reason() = ESP_RST_SW;
    assert(esp_reset_reason() == ESP_RST_SW);

    // Chip info
    esp_chip_info_t ci;
    esp_chip_info(&ci);
    assert(ci.model == ESP_CHIP_MODEL_ESP32);
    assert(ci.cores == 2);
    assert(ci.features & CHIP_FEATURE_WIFI_BGN);
    assert(ci.features & CHIP_FEATURE_BLE);

    // Heap
    assert(esp_get_free_heap_size() == 256000);
    assert(esp_get_minimum_free_heap_size() == 200000);
    esp32emu::system::_free_heap() = 128000;
    assert(esp_get_free_heap_size() == 128000);

    // Restart
    assert(!esp32emu::system::_restart_called());
    esp_restart();
    assert(esp32emu::system::_restart_called());

    // Random
    (void)esp_random();
    uint8_t buf[16] = {};
    esp_fill_random(buf, sizeof(buf));
    // Just check it doesn't crash

    // IDF version
    assert(esp_get_idf_version() != nullptr);

    // Reset
    esp32emu::system::reset();
    assert(esp_reset_reason() == ESP_RST_POWERON);
    assert(!esp32emu::system::_restart_called());

    printf("test_esp_system: ALL PASSED\n");
    return 0;
}
