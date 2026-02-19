#pragma once
// esp32emu: ESP-IDF partition table API mock
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

typedef int esp_err_t;
#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif
#define ESP_ERR_NOT_FOUND       0x105
#define ESP_ERR_INVALID_SIZE    0x106

typedef enum {
    ESP_PARTITION_TYPE_APP = 0x00,
    ESP_PARTITION_TYPE_DATA = 0x01,
} esp_partition_type_t;

typedef enum {
    ESP_PARTITION_SUBTYPE_APP_FACTORY = 0x00,
    ESP_PARTITION_SUBTYPE_APP_OTA_0 = 0x10,
    ESP_PARTITION_SUBTYPE_APP_OTA_1 = 0x11,
    ESP_PARTITION_SUBTYPE_DATA_NVS = 0x02,
    ESP_PARTITION_SUBTYPE_DATA_PHY = 0x01,
    ESP_PARTITION_SUBTYPE_DATA_OTA = 0x00,
    ESP_PARTITION_SUBTYPE_DATA_SPIFFS = 0x82,
    ESP_PARTITION_SUBTYPE_ANY = 0xFF,
} esp_partition_subtype_t;

typedef struct {
    uint32_t type;          // esp_partition_type_t as uint32
    uint32_t subtype;       // esp_partition_subtype_t as uint32
    uint32_t address;
    uint32_t size;
    char label[17];
    bool encrypted;
} esp_partition_t;

#ifdef __cplusplus
}
#endif

// ── C++ implementation ──
namespace esp32emu {
namespace partition {

struct PartitionStore {
    std::vector<esp_partition_t> partitions;
    std::vector<std::vector<uint8_t>> data;  // backing storage per partition

    PartitionStore() { reset(); }

    void reset() {
        partitions.clear();
        data.clear();
        // Default partition table
        add("factory", ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, 0x10000, 0x100000);
        add("ota_0", ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, 0x110000, 0x100000);
        add("ota_1", ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, 0x210000, 0x100000);
        add("nvs", ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, 0x9000, 0x5000);
        add("otadata", ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, 0xE000, 0x2000);
        add("spiffs", ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, 0x310000, 0xF0000);
    }

    void add(const char* label, uint32_t type, uint32_t subtype, uint32_t addr, uint32_t sz) {
        esp_partition_t p{};
        p.type = type;
        p.subtype = subtype;
        p.address = addr;
        p.size = sz;
        p.encrypted = false;
        strncpy(p.label, label, 16);
        p.label[16] = '\0';
        partitions.push_back(p);
        data.push_back(std::vector<uint8_t>(sz, 0xFF));
    }
};

inline PartitionStore& store() {
    static PartitionStore s;
    return s;
}

inline void reset() { store().reset(); }

} // namespace partition
} // namespace esp32emu

inline const esp_partition_t* esp_partition_find_first(
    esp_partition_type_t type, esp_partition_subtype_t subtype, const char* label)
{
    auto& parts = esp32emu::partition::store().partitions;
    for (auto& p : parts) {
        if (p.type != (uint32_t)type) continue;
        if (subtype != ESP_PARTITION_SUBTYPE_ANY && p.subtype != (uint32_t)subtype) continue;
        if (label && strcmp(p.label, label) != 0) continue;
        return &p;
    }
    return nullptr;
}

inline esp_err_t esp_partition_read(const esp_partition_t* part, size_t src_offset, void* dst, size_t size) {
    if (!part || !dst) return ESP_FAIL;
    auto& store = esp32emu::partition::store();
    for (size_t i = 0; i < store.partitions.size(); i++) {
        if (&store.partitions[i] == part) {
            if (src_offset + size > part->size) return ESP_ERR_INVALID_SIZE;
            memcpy(dst, store.data[i].data() + src_offset, size);
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

inline esp_err_t esp_partition_write(const esp_partition_t* part, size_t dst_offset, const void* src, size_t size) {
    if (!part || !src) return ESP_FAIL;
    auto& store = esp32emu::partition::store();
    for (size_t i = 0; i < store.partitions.size(); i++) {
        if (&store.partitions[i] == part) {
            if (dst_offset + size > part->size) return ESP_ERR_INVALID_SIZE;
            memcpy(store.data[i].data() + dst_offset, src, size);
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

inline esp_err_t esp_partition_erase_range(const esp_partition_t* part, size_t offset, size_t size) {
    if (!part) return ESP_FAIL;
    auto& store = esp32emu::partition::store();
    for (size_t i = 0; i < store.partitions.size(); i++) {
        if (&store.partitions[i] == part) {
            if (offset + size > part->size) return ESP_ERR_INVALID_SIZE;
            memset(store.data[i].data() + offset, 0xFF, size);
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}
