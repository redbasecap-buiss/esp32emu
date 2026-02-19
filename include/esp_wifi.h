#pragma once
// esp32emu — ESP-IDF WiFi API mock
// Provides esp_wifi_init, start, stop, connect, disconnect, scan, and config.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <functional>

typedef int esp_err_t;
#ifndef ESP_OK
#define ESP_OK    0
#define ESP_FAIL -1
#define ESP_ERR_WIFI_NOT_INIT    0x3001
#define ESP_ERR_WIFI_NOT_STARTED 0x3002
#endif

typedef enum {
    WIFI_MODE_NULL = 0,
    WIFI_MODE_STA,
    WIFI_MODE_AP,
    WIFI_MODE_APSTA,
    WIFI_MODE_MAX
} wifi_mode_t;

typedef enum {
    WIFI_IF_STA = 0,
    WIFI_IF_AP,
    WIFI_IF_MAX
} wifi_interface_t;

typedef enum {
    WIFI_AUTH_OPEN = 0,
    WIFI_AUTH_WEP,
    WIFI_AUTH_WPA_PSK,
    WIFI_AUTH_WPA2_PSK,
    WIFI_AUTH_WPA_WPA2_PSK,
    WIFI_AUTH_WPA3_PSK,
    WIFI_AUTH_MAX
} wifi_auth_mode_t;

typedef struct {
    uint8_t ssid[32];
    uint8_t password[64];
    uint8_t ssid_len;
    uint8_t channel;
    wifi_auth_mode_t threshold_authmode;
} wifi_sta_config_t;

typedef struct {
    uint8_t ssid[32];
    uint8_t password[64];
    uint8_t ssid_len;
    uint8_t channel;
    wifi_auth_mode_t authmode;
    uint8_t max_connection;
} wifi_ap_config_t;

typedef union {
    wifi_sta_config_t sta;
    wifi_ap_config_t  ap;
} wifi_config_t;

typedef struct {
    // Simplified init config
    void* event_handler;
    void* osi_funcs;
    int static_rx_buf_num;
    int dynamic_rx_buf_num;
} wifi_init_config_t;

#define WIFI_INIT_CONFIG_DEFAULT() wifi_init_config_t{nullptr, nullptr, 10, 32}

typedef struct {
    uint8_t bssid[6];
    uint8_t ssid[33];
    uint8_t primary;
    int8_t  rssi;
    wifi_auth_mode_t authmode;
} wifi_ap_record_t;

namespace esp32emu {
namespace wifi_idf {

struct State {
    bool initialized = false;
    bool started = false;
    bool connected = false;
    wifi_mode_t mode = WIFI_MODE_NULL;
    wifi_config_t sta_config{};
    wifi_config_t ap_config{};
    std::vector<wifi_ap_record_t> scan_results;
    uint8_t ip[4] = {192, 168, 4, 100};
};

inline State& state() {
    static State s;
    return s;
}

inline void reset() {
    state() = State{};
}

inline void addScanResult(const char* ssid, int8_t rssi, wifi_auth_mode_t auth = WIFI_AUTH_WPA2_PSK) {
    wifi_ap_record_t rec{};
    strncpy((char*)rec.ssid, ssid, 32);
    rec.rssi = rssi;
    rec.authmode = auth;
    rec.primary = 6;
    state().scan_results.push_back(rec);
}

} // namespace wifi_idf
} // namespace esp32emu

inline esp_err_t esp_wifi_init(const wifi_init_config_t* config) {
    (void)config;
    if (esp32emu::wifi_idf::state().initialized) return ESP_OK;
    esp32emu::wifi_idf::state().initialized = true;
    fprintf(stderr, "[esp32emu] esp_wifi_init\n");
    return ESP_OK;
}

inline esp_err_t esp_wifi_deinit() {
    esp32emu::wifi_idf::state().initialized = false;
    esp32emu::wifi_idf::state().started = false;
    return ESP_OK;
}

inline esp_err_t esp_wifi_set_mode(wifi_mode_t mode) {
    auto& s = esp32emu::wifi_idf::state();
    if (!s.initialized) return ESP_ERR_WIFI_NOT_INIT;
    s.mode = mode;
    fprintf(stderr, "[esp32emu] esp_wifi_set_mode: %d\n", (int)mode);
    return ESP_OK;
}

inline esp_err_t esp_wifi_get_mode(wifi_mode_t* mode) {
    *mode = esp32emu::wifi_idf::state().mode;
    return ESP_OK;
}

inline esp_err_t esp_wifi_set_config(wifi_interface_t iface, wifi_config_t* conf) {
    auto& s = esp32emu::wifi_idf::state();
    if (iface == WIFI_IF_STA) {
        s.sta_config = *conf;
        fprintf(stderr, "[esp32emu] esp_wifi_set_config STA: ssid='%s'\n", conf->sta.ssid);
    } else {
        s.ap_config = *conf;
        fprintf(stderr, "[esp32emu] esp_wifi_set_config AP: ssid='%s'\n", conf->ap.ssid);
    }
    return ESP_OK;
}

inline esp_err_t esp_wifi_get_config(wifi_interface_t iface, wifi_config_t* conf) {
    auto& s = esp32emu::wifi_idf::state();
    *conf = (iface == WIFI_IF_STA) ? s.sta_config : s.ap_config;
    return ESP_OK;
}

inline esp_err_t esp_wifi_start() {
    auto& s = esp32emu::wifi_idf::state();
    if (!s.initialized) return ESP_ERR_WIFI_NOT_INIT;
    s.started = true;
    fprintf(stderr, "[esp32emu] esp_wifi_start\n");
    return ESP_OK;
}

inline esp_err_t esp_wifi_stop() {
    auto& s = esp32emu::wifi_idf::state();
    s.started = false;
    s.connected = false;
    fprintf(stderr, "[esp32emu] esp_wifi_stop\n");
    return ESP_OK;
}

inline esp_err_t esp_wifi_connect() {
    auto& s = esp32emu::wifi_idf::state();
    if (!s.started) return ESP_ERR_WIFI_NOT_STARTED;
    s.connected = true;
    fprintf(stderr, "[esp32emu] esp_wifi_connect: connected to '%s'\n", s.sta_config.sta.ssid);
    return ESP_OK;
}

inline esp_err_t esp_wifi_disconnect() {
    esp32emu::wifi_idf::state().connected = false;
    fprintf(stderr, "[esp32emu] esp_wifi_disconnect\n");
    return ESP_OK;
}

inline esp_err_t esp_wifi_scan_start(void* config, bool block) {
    (void)config;
    (void)block;
    fprintf(stderr, "[esp32emu] esp_wifi_scan_start\n");
    return ESP_OK;
}

inline esp_err_t esp_wifi_scan_get_ap_num(uint16_t* number) {
    *number = (uint16_t)esp32emu::wifi_idf::state().scan_results.size();
    return ESP_OK;
}

inline esp_err_t esp_wifi_scan_get_ap_records(uint16_t* number, wifi_ap_record_t* ap_records) {
    auto& results = esp32emu::wifi_idf::state().scan_results;
    uint16_t count = (*number < (uint16_t)results.size()) ? *number : (uint16_t)results.size();
    for (uint16_t i = 0; i < count; i++) {
        ap_records[i] = results[i];
    }
    *number = count;
    return ESP_OK;
}

// Test helper: check connection state
namespace esp32emu {
namespace wifi_idf {
inline bool isConnected() { return state().connected; }
inline bool isStarted() { return state().started; }
} // namespace wifi_idf
} // namespace esp32emu
