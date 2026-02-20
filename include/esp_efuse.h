// esp32emu — ESP eFuse mock
// Provides chip revision, flash info, and custom eFuse field reading.
#pragma once
#include <cstdint>
#include <cstring>

#ifndef ESP_OK
#define ESP_OK   0
#define ESP_FAIL -1
#endif
typedef int esp_err_t;

typedef enum {
    ESP_EFUSE_BLOCK0 = 0,
    ESP_EFUSE_BLOCK1,
    ESP_EFUSE_BLOCK2,
    ESP_EFUSE_BLOCK3,
    ESP_EFUSE_BLOCK_KEY0 = ESP_EFUSE_BLOCK1,
    ESP_EFUSE_BLOCK_KEY1 = ESP_EFUSE_BLOCK2,
    ESP_EFUSE_BLOCK_KEY2 = ESP_EFUSE_BLOCK3,
} esp_efuse_block_t;

namespace esp32emu {
namespace efuse {

struct EfuseState {
    uint8_t chip_revision = 3;        // Major.Minor → 3 = v3.0
    uint8_t chip_revision_minor = 0;
    uint32_t flash_size_mb = 4;
    uint8_t blocks[4][32] = {};       // Simplified: 4 blocks × 32 bytes

    static EfuseState& instance() {
        static EfuseState s;
        return s;
    }

    void reset() {
        chip_revision = 3;
        chip_revision_minor = 0;
        flash_size_mb = 4;
        std::memset(blocks, 0, sizeof(blocks));
    }
};

} // namespace efuse
} // namespace esp32emu

// Get chip revision (major)
inline uint8_t esp_efuse_get_chip_ver() {
    return esp32emu::efuse::EfuseState::instance().chip_revision;
}

// ESP-IDF v5+ API
inline unsigned esp_efuse_get_major_chip_version() {
    return esp32emu::efuse::EfuseState::instance().chip_revision;
}

inline unsigned esp_efuse_get_minor_chip_version() {
    return esp32emu::efuse::EfuseState::instance().chip_revision_minor;
}

// Read a block of eFuse data
inline esp_err_t esp_efuse_read_block(esp_efuse_block_t blk, void* dst, size_t offset_bits, size_t size_bits) {
    if (!dst || (int)blk < 0 || (int)blk > 3) return ESP_FAIL;
    size_t byte_off = offset_bits / 8;
    size_t byte_len = (size_bits + 7) / 8;
    auto& state = esp32emu::efuse::EfuseState::instance();
    if (byte_off + byte_len > 32) return ESP_FAIL;
    std::memcpy(dst, &state.blocks[(int)blk][byte_off], byte_len);
    return ESP_OK;
}

// Write eFuse block (emulator: freely writable, real HW is one-time)
inline esp_err_t esp_efuse_write_block(esp_efuse_block_t blk, const void* src, size_t offset_bits, size_t size_bits) {
    if (!src || (int)blk < 0 || (int)blk > 3) return ESP_FAIL;
    size_t byte_off = offset_bits / 8;
    size_t byte_len = (size_bits + 7) / 8;
    auto& state = esp32emu::efuse::EfuseState::instance();
    if (byte_off + byte_len > 32) return ESP_FAIL;
    std::memcpy(&state.blocks[(int)blk][byte_off], src, byte_len);
    return ESP_OK;
}
