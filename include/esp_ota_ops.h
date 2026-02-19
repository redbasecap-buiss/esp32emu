#pragma once
// esp32emu: ESP-IDF OTA operations mock
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>
#include "esp_partition.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif
#define ESP_ERR_OTA_BASE                0x1500
#define ESP_ERR_OTA_PARTITION_CONFLICT  (ESP_ERR_OTA_BASE + 1)
#define ESP_ERR_OTA_VALIDATE_FAILED     (ESP_ERR_OTA_BASE + 3)

typedef uint32_t esp_ota_handle_t;

typedef struct {
    char version[32];
    char project_name[32];
    char date[16];
    char time[16];
    char idf_ver[32];
} esp_app_desc_t;

#ifdef __cplusplus
}
#endif

namespace esp32emu {
namespace ota {

struct OtaState {
    bool in_progress = false;
    const esp_partition_t* target = nullptr;
    std::vector<uint8_t> written_data;
    uint32_t handle_counter = 1;
    const esp_partition_t* boot_partition = nullptr;
    const esp_partition_t* running_partition = nullptr;
    bool set_boot_called = false;
    esp_app_desc_t app_desc = {"1.0.0", "esp32emu", __DATE__, __TIME__, "v5.1-esp32emu"};
};

inline OtaState& state() {
    static OtaState s;
    return s;
}

inline void reset() {
    state() = OtaState{};
}

} // namespace ota
} // namespace esp32emu

inline esp_err_t esp_ota_begin(const esp_partition_t* partition, size_t /*image_size*/, esp_ota_handle_t* out_handle) {
    if (!partition || !out_handle) return ESP_FAIL;
    auto& s = esp32emu::ota::state();
    s.in_progress = true;
    s.target = partition;
    s.written_data.clear();
    *out_handle = s.handle_counter++;
    fprintf(stderr, "[esp32emu] OTA begin on partition '%s'\n", partition->label);
    return ESP_OK;
}

inline esp_err_t esp_ota_write(esp_ota_handle_t /*handle*/, const void* data, size_t size) {
    auto& s = esp32emu::ota::state();
    if (!s.in_progress) return ESP_FAIL;
    const uint8_t* p = (const uint8_t*)data;
    s.written_data.insert(s.written_data.end(), p, p + size);
    return ESP_OK;
}

inline esp_err_t esp_ota_end(esp_ota_handle_t /*handle*/) {
    auto& s = esp32emu::ota::state();
    if (!s.in_progress) return ESP_FAIL;
    s.in_progress = false;
    fprintf(stderr, "[esp32emu] OTA end, wrote %zu bytes\n", s.written_data.size());
    return ESP_OK;
}

inline esp_err_t esp_ota_set_boot_partition(const esp_partition_t* partition) {
    if (!partition) return ESP_FAIL;
    auto& s = esp32emu::ota::state();
    s.boot_partition = partition;
    s.set_boot_called = true;
    fprintf(stderr, "[esp32emu] OTA boot partition set to '%s'\n", partition->label);
    return ESP_OK;
}

inline const esp_partition_t* esp_ota_get_boot_partition() {
    auto& s = esp32emu::ota::state();
    if (s.boot_partition) return s.boot_partition;
    return esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, nullptr);
}

inline const esp_partition_t* esp_ota_get_running_partition() {
    auto& s = esp32emu::ota::state();
    if (s.running_partition) return s.running_partition;
    return esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, nullptr);
}

inline const esp_partition_t* esp_ota_get_next_update_partition(const esp_partition_t* /*from*/) {
    // Return ota_0 or ota_1
    auto* p = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, nullptr);
    if (p) return p;
    return esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, nullptr);
}

inline esp_err_t esp_ota_get_partition_description(const esp_partition_t* /*partition*/, esp_app_desc_t* app_desc) {
    if (!app_desc) return ESP_FAIL;
    *app_desc = esp32emu::ota::state().app_desc;
    return ESP_OK;
}
