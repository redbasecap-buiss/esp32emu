// esp32emu test — ESP-IDF SPI Flash API
#include <cassert>
#include <cstring>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "esp_flash.h"

int main() {
    esp32emu::flash_reset();

    // Test get_size
    {
        uint32_t size = 0;
        assert(esp_flash_get_size(nullptr, &size) == ESP_OK);
        assert(size == 4 * 1024 * 1024);
    }

    // Test read from erased flash (all 0xFF)
    {
        uint8_t buf[16];
        assert(esp_flash_read(nullptr, buf, 0, 16) == ESP_OK);
        for (int i = 0; i < 16; i++) assert(buf[i] == 0xFF);
    }

    // Test write (flash semantics: can only clear bits)
    {
        uint8_t data[4] = {0xAB, 0xCD, 0xEF, 0x01};
        assert(esp_flash_write(nullptr, data, 0x1000, 4) == ESP_OK);

        uint8_t buf[4];
        assert(esp_flash_read(nullptr, buf, 0x1000, 4) == ESP_OK);
        assert(memcmp(buf, data, 4) == 0);
    }

    // Test write can only clear bits (AND behavior)
    {
        uint8_t first[4] = {0xFF, 0xFF, 0x00, 0x00};
        esp_flash_write(nullptr, first, 0x2000, 4);

        uint8_t second[4] = {0x0F, 0xF0, 0xFF, 0xFF};
        esp_flash_write(nullptr, second, 0x2000, 4);

        uint8_t buf[4];
        esp_flash_read(nullptr, buf, 0x2000, 4);
        assert(buf[0] == 0x0F); // 0xFF & 0x0F
        assert(buf[1] == 0xF0); // 0xFF & 0xF0
        assert(buf[2] == 0x00); // 0x00 & 0xFF
        assert(buf[3] == 0x00); // 0x00 & 0xFF
    }

    // Test erase region
    {
        uint8_t data[4] = {0x12, 0x34, 0x56, 0x78};
        esp_flash_write(nullptr, data, 0x3000, 4);
        assert(esp_flash_erase_region(nullptr, 0x3000, 4096) == ESP_OK);

        uint8_t buf[4];
        esp_flash_read(nullptr, buf, 0x3000, 4);
        for (int i = 0; i < 4; i++) assert(buf[i] == 0xFF);
    }

    // Test erase_chip
    {
        uint8_t data[4] = {0x11, 0x22, 0x33, 0x44};
        esp_flash_write(nullptr, data, 0, 4);
        assert(esp_flash_erase_chip(nullptr) == ESP_OK);

        uint8_t buf[4];
        esp_flash_read(nullptr, buf, 0, 4);
        for (int i = 0; i < 4; i++) assert(buf[i] == 0xFF);
    }

    // Test out-of-bounds
    {
        uint8_t buf[4];
        uint32_t size = 4 * 1024 * 1024;
        assert(esp_flash_read(nullptr, buf, size, 1) == ESP_ERR_INVALID_SIZE);
        assert(esp_flash_write(nullptr, buf, size, 1) == ESP_ERR_INVALID_SIZE);
        assert(esp_flash_erase_region(nullptr, size, 1) == ESP_ERR_INVALID_SIZE);
    }

    // Test init
    {
        assert(esp_flash_init(nullptr) == ESP_OK);
    }

    printf("test_esp_flash: all assertions passed\n");
    return 0;
}
