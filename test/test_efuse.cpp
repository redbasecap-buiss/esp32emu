// esp32emu — eFuse mock test
#include "esp_efuse.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    auto& state = esp32emu::efuse::EfuseState::instance();
    state.reset();

    // Chip version
    assert(esp_efuse_get_chip_ver() == 3);
    assert(esp_efuse_get_major_chip_version() == 3);
    assert(esp_efuse_get_minor_chip_version() == 0);

    // Modify and check
    state.chip_revision = 4;
    state.chip_revision_minor = 1;
    assert(esp_efuse_get_major_chip_version() == 4);
    assert(esp_efuse_get_minor_chip_version() == 1);

    // Write and read eFuse block
    uint8_t data[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    assert(esp_efuse_write_block(ESP_EFUSE_BLOCK0, data, 0, 32) == ESP_OK);

    uint8_t out[4] = {};
    assert(esp_efuse_read_block(ESP_EFUSE_BLOCK0, out, 0, 32) == ESP_OK);
    assert(memcmp(data, out, 4) == 0);

    // Read with offset
    uint8_t partial[2] = {};
    assert(esp_efuse_read_block(ESP_EFUSE_BLOCK0, partial, 16, 16) == ESP_OK);
    assert(partial[0] == 0xBE && partial[1] == 0xEF);

    // Different blocks are independent
    uint8_t data2[4] = {0x11, 0x22, 0x33, 0x44};
    assert(esp_efuse_write_block(ESP_EFUSE_BLOCK1, data2, 0, 32) == ESP_OK);
    uint8_t out2[4] = {};
    assert(esp_efuse_read_block(ESP_EFUSE_BLOCK1, out2, 0, 32) == ESP_OK);
    assert(memcmp(data2, out2, 4) == 0);
    // Block0 unchanged
    assert(esp_efuse_read_block(ESP_EFUSE_BLOCK0, out, 0, 32) == ESP_OK);
    assert(memcmp(data, out, 4) == 0);

    // Error cases
    assert(esp_efuse_read_block(ESP_EFUSE_BLOCK0, nullptr, 0, 8) == ESP_FAIL);
    assert(esp_efuse_write_block(ESP_EFUSE_BLOCK0, nullptr, 0, 8) == ESP_FAIL);
    assert(esp_efuse_read_block((esp_efuse_block_t)99, out, 0, 8) == ESP_FAIL);

    // Out of bounds
    assert(esp_efuse_read_block(ESP_EFUSE_BLOCK0, out, 248, 16) == ESP_FAIL);

    state.reset();
    printf("test_efuse: all assertions passed\n");
    return 0;
}
