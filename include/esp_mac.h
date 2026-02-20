#pragma once
/*  esp_mac.h — ESP-IDF MAC address helpers mock for esp32emu */

#include "esp_err.h"
#include <cstdint>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_MAC_WIFI_STA = 0,
    ESP_MAC_WIFI_SOFTAP,
    ESP_MAC_BT,
    ESP_MAC_ETH,
} esp_mac_type_t;

/// Read base MAC address (returns a deterministic fake)
inline esp_err_t esp_base_mac_addr_get(uint8_t* mac) {
    if (!mac) return ESP_ERR_INVALID_ARG;
    // Deterministic mock MAC: AA:BB:CC:DD:EE:00
    const uint8_t base[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x00};
    memcpy(mac, base, 6);
    return ESP_OK;
}

/// Set custom base MAC address (accepted but ignored in emulation)
inline esp_err_t esp_base_mac_addr_set(const uint8_t* /*mac*/) {
    return ESP_OK;
}

/// Read MAC for a specific interface
inline esp_err_t esp_read_mac(uint8_t* mac, esp_mac_type_t type) {
    if (!mac) return ESP_ERR_INVALID_ARG;
    const uint8_t base[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x00};
    memcpy(mac, base, 6);
    mac[5] = (uint8_t)type; // vary last byte by interface
    return ESP_OK;
}

/// Derive local MAC from universal MAC
inline esp_err_t esp_derive_local_mac(uint8_t* local_mac, const uint8_t* universal_mac) {
    if (!local_mac || !universal_mac) return ESP_ERR_INVALID_ARG;
    memcpy(local_mac, universal_mac, 6);
    local_mac[0] |= 0x02; // set locally administered bit
    return ESP_OK;
}

#ifdef __cplusplus
}
#endif
