#pragma once
// esp32emu — ESP HTTPS OTA mock
// Simulates OTA firmware update over HTTPS for host-side testing.

#include "esp_err.h"
#include "esp_http_client.h"
#include <cstdio>
#include <cstdint>
#include <string>

#ifndef ESP_ERR_HTTPS_OTA_IN_PROGRESS
#define ESP_ERR_HTTPS_OTA_IN_PROGRESS 0x1010
#endif

typedef struct {
    esp_http_client_config_t http_config;
    bool bulk_flash_erase;
    bool partial_http_download;
    int max_http_request_size;
} esp_https_ota_config_t;

typedef void* esp_https_ota_handle_t;

namespace esp32emu {

struct OtaState {
    bool inProgress = false;
    bool completed = false;
    int totalSize = 102400;   // 100KB simulated firmware
    int downloaded = 0;
    bool shouldFail = false;
    std::string url;

    static OtaState& instance() {
        static OtaState inst;
        return inst;
    }

    void reset() {
        inProgress = false;
        completed = false;
        totalSize = 102400;
        downloaded = 0;
        shouldFail = false;
        url.clear();
    }
};

} // namespace esp32emu

// Simple one-shot OTA
inline esp_err_t esp_https_ota(const esp_https_ota_config_t* config) {
    auto& state = esp32emu::OtaState::instance();
    if (config && config->http_config.url) {
        state.url = config->http_config.url;
        printf("[esp32emu] HTTPS OTA: downloading from %s\n", config->http_config.url);
    }
    if (state.shouldFail) {
        printf("[esp32emu] HTTPS OTA: simulated failure\n");
        return ESP_FAIL;
    }
    state.completed = true;
    printf("[esp32emu] HTTPS OTA: update successful (simulated)\n");
    return ESP_OK;
}

// Begin/perform/finish API for progress tracking
inline esp_err_t esp_https_ota_begin(const esp_https_ota_config_t* config, esp_https_ota_handle_t* handle) {
    auto& state = esp32emu::OtaState::instance();
    bool fail = state.shouldFail;
    state.reset();
    state.shouldFail = fail;
    state.inProgress = true;
    if (config && config->http_config.url) {
        state.url = config->http_config.url;
        printf("[esp32emu] HTTPS OTA begin: %s\n", config->http_config.url);
    }
    static int dummy_handle = 1;
    *handle = &dummy_handle;
    return state.shouldFail ? ESP_FAIL : ESP_OK;
}

inline esp_err_t esp_https_ota_perform(esp_https_ota_handle_t handle) {
    (void)handle;
    auto& state = esp32emu::OtaState::instance();
    if (!state.inProgress) return ESP_FAIL;
    if (state.shouldFail) return ESP_FAIL;

    // Simulate downloading in chunks
    int chunk = state.totalSize / 4;
    state.downloaded += chunk;
    if (state.downloaded >= state.totalSize) {
        state.downloaded = state.totalSize;
        return ESP_OK;  // Download complete
    }
    return ESP_ERR_HTTPS_OTA_IN_PROGRESS;
}

inline bool esp_https_ota_is_complete_data_received(esp_https_ota_handle_t handle) {
    (void)handle;
    auto& state = esp32emu::OtaState::instance();
    return state.downloaded >= state.totalSize;
}

inline esp_err_t esp_https_ota_finish(esp_https_ota_handle_t handle) {
    (void)handle;
    auto& state = esp32emu::OtaState::instance();
    state.inProgress = false;
    state.completed = true;
    printf("[esp32emu] HTTPS OTA: finished\n");
    return ESP_OK;
}

inline int esp_https_ota_get_image_len_read(esp_https_ota_handle_t handle) {
    (void)handle;
    return esp32emu::OtaState::instance().downloaded;
}

inline int esp_https_ota_get_image_size(esp_https_ota_handle_t handle) {
    (void)handle;
    return esp32emu::OtaState::instance().totalSize;
}

