#pragma once
// esp32emu: ESP-IDF system API mock
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

typedef enum {
    ESP_RST_UNKNOWN = 0,
    ESP_RST_POWERON = 1,
    ESP_RST_EXT = 2,
    ESP_RST_SW = 3,
    ESP_RST_PANIC = 4,
    ESP_RST_INT_WDT = 5,
    ESP_RST_TASK_WDT = 6,
    ESP_RST_WDT = 7,
    ESP_RST_DEEPSLEEP = 8,
    ESP_RST_BROWNOUT = 9,
    ESP_RST_SDIO = 10,
} esp_reset_reason_t;

typedef enum {
    ESP_CHIP_MODEL_ESP32 = 1,
    ESP_CHIP_MODEL_ESP32S2 = 2,
    ESP_CHIP_MODEL_ESP32S3 = 9,
    ESP_CHIP_MODEL_ESP32C3 = 5,
    ESP_CHIP_MODEL_ESP32C2 = 12,
    ESP_CHIP_MODEL_ESP32C6 = 13,
    ESP_CHIP_MODEL_ESP32H2 = 16,
} esp_chip_model_t;

typedef struct {
    esp_chip_model_t model;
    uint32_t features;
    uint16_t revision;
    uint8_t cores;
} esp_chip_info_t;

#define CHIP_FEATURE_EMB_FLASH  (1 << 0)
#define CHIP_FEATURE_WIFI_BGN   (1 << 1)
#define CHIP_FEATURE_BLE        (1 << 4)
#define CHIP_FEATURE_BT         (1 << 5)
#define CHIP_FEATURE_EMB_PSRAM  (1 << 6)

// ── Test helpers ──
namespace esp32emu {
namespace system {

inline esp_reset_reason_t& _reset_reason() {
    static esp_reset_reason_t reason = ESP_RST_POWERON;
    return reason;
}

inline esp_chip_info_t& _chip_info() {
    static esp_chip_info_t info = {
        ESP_CHIP_MODEL_ESP32,
        CHIP_FEATURE_WIFI_BGN | CHIP_FEATURE_BLE | CHIP_FEATURE_BT,
        300,
        2
    };
    return info;
}

inline uint32_t& _free_heap() {
    static uint32_t heap = 256000;
    return heap;
}

inline uint32_t& _min_free_heap() {
    static uint32_t heap = 200000;
    return heap;
}

inline bool& _restart_called() {
    static bool called = false;
    return called;
}

inline void reset() {
    _reset_reason() = ESP_RST_POWERON;
    auto& ci = _chip_info();
    ci.model = ESP_CHIP_MODEL_ESP32;
    ci.features = CHIP_FEATURE_WIFI_BGN | CHIP_FEATURE_BLE | CHIP_FEATURE_BT;
    ci.revision = 300;
    ci.cores = 2;
    _free_heap() = 256000;
    _min_free_heap() = 200000;
    _restart_called() = false;
}

} // namespace system
} // namespace esp32emu

inline esp_reset_reason_t esp_reset_reason() {
    return esp32emu::system::_reset_reason();
}

inline void esp_chip_info(esp_chip_info_t* out) {
    if (out) *out = esp32emu::system::_chip_info();
}

inline uint32_t esp_get_free_heap_size() {
    return esp32emu::system::_free_heap();
}

inline uint32_t esp_get_minimum_free_heap_size() {
    return esp32emu::system::_min_free_heap();
}

inline void esp_restart() {
    esp32emu::system::_restart_called() = true;
    fprintf(stderr, "[esp32emu] esp_restart() called\n");
}

inline uint32_t esp_random() {
    return (uint32_t)rand();
}

inline void esp_fill_random(void* buf, size_t len) {
    uint8_t* p = (uint8_t*)buf;
    for (size_t i = 0; i < len; i++) p[i] = (uint8_t)(rand() & 0xFF);
}

inline const char* esp_get_idf_version() {
    return "v5.1-esp32emu";
}
