#pragma once
// esp32emu — ESP-IDF SPI Flash API mock (esp_flash)

#include <esp_err.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>

typedef struct {
    uint32_t size;          // Flash size in bytes
    uint32_t sector_size;   // Sector size (typically 4096)
    uint32_t block_size;    // Block size (typically 65536)
} esp_flash_t;

// Default "main" flash chip
inline esp_flash_t* esp_flash_default_chip = nullptr;

namespace esp32emu {
    // Simulated flash storage
    inline std::vector<uint8_t>& flash_storage() {
        static std::vector<uint8_t> storage(4 * 1024 * 1024, 0xFF); // 4MB default
        return storage;
    }
    inline bool flash_initialized() {
        static bool init = false;
        return init;
    }
    inline void flash_set_initialized(bool v) {
        static bool& init = ([]() -> bool& { static bool i = false; return i; })();
        init = v;
        (void)v; // suppress
    }
}

inline esp_err_t esp_flash_init(esp_flash_t* /*chip*/) {
    return ESP_OK;
}

inline esp_err_t esp_flash_get_size(esp_flash_t* /*chip*/, uint32_t* out_size) {
    *out_size = (uint32_t)esp32emu::flash_storage().size();
    return ESP_OK;
}

inline esp_err_t esp_flash_read(esp_flash_t* /*chip*/, void* buffer, uint32_t address, uint32_t length) {
    auto& storage = esp32emu::flash_storage();
    if (address + length > storage.size()) return ESP_ERR_INVALID_SIZE;
    memcpy(buffer, storage.data() + address, length);
    return ESP_OK;
}

inline esp_err_t esp_flash_write(esp_flash_t* /*chip*/, const void* buffer, uint32_t address, uint32_t length) {
    auto& storage = esp32emu::flash_storage();
    if (address + length > storage.size()) return ESP_ERR_INVALID_SIZE;
    // Flash write: can only clear bits (AND with existing)
    const uint8_t* src = (const uint8_t*)buffer;
    for (uint32_t i = 0; i < length; i++) {
        storage[address + i] &= src[i];
    }
    return ESP_OK;
}

inline esp_err_t esp_flash_erase_region(esp_flash_t* /*chip*/, uint32_t start, uint32_t length) {
    auto& storage = esp32emu::flash_storage();
    if (start + length > storage.size()) return ESP_ERR_INVALID_SIZE;
    memset(storage.data() + start, 0xFF, length);
    return ESP_OK;
}

inline esp_err_t esp_flash_erase_chip(esp_flash_t* /*chip*/) {
    auto& storage = esp32emu::flash_storage();
    memset(storage.data(), 0xFF, storage.size());
    return ESP_OK;
}

// Test helpers
namespace esp32emu {
    inline void flash_reset() {
        auto& s = flash_storage();
        memset(s.data(), 0xFF, s.size());
    }
}
