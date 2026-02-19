#pragma once
// esp32emu — ESP-IDF Network Interface (esp_netif) mock
// Provides esp_netif_init, create_default_wifi_sta/ap, get IP info.

#include <cstdint>
#include <cstdio>
#include <cstring>

typedef int esp_err_t;
#ifndef ESP_OK
#define ESP_OK    0
#define ESP_FAIL -1
#endif

typedef struct esp_netif_obj* esp_netif_t;

struct esp_netif_obj {
    uint8_t ip[4];
    uint8_t gw[4];
    uint8_t netmask[4];
    bool is_sta;
};

typedef struct {
    struct {
        uint32_t addr;
    } ip;
    struct {
        uint32_t addr;
    } gw;
    struct {
        uint32_t addr;
    } netmask;
} esp_netif_ip_info_t;

namespace esp32emu {
namespace netif {

inline esp_netif_obj& staObj() {
    static esp_netif_obj obj{{192,168,4,100}, {192,168,4,1}, {255,255,255,0}, true};
    return obj;
}

inline esp_netif_obj& apObj() {
    static esp_netif_obj obj{{192,168,4,1}, {192,168,4,1}, {255,255,255,0}, false};
    return obj;
}

inline bool& initialized() {
    static bool init = false;
    return init;
}

inline void reset() {
    initialized() = false;
    staObj() = {{192,168,4,100}, {192,168,4,1}, {255,255,255,0}, true};
    apObj() = {{192,168,4,1}, {192,168,4,1}, {255,255,255,0}, false};
}

inline void setStaIp(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    auto& s = staObj();
    s.ip[0] = a; s.ip[1] = b; s.ip[2] = c; s.ip[3] = d;
}

} // namespace netif
} // namespace esp32emu

inline esp_err_t esp_netif_init() {
    esp32emu::netif::initialized() = true;
    fprintf(stderr, "[esp32emu] esp_netif_init\n");
    return ESP_OK;
}

inline esp_err_t esp_netif_deinit() {
    esp32emu::netif::initialized() = false;
    return ESP_OK;
}

inline esp_netif_t esp_netif_create_default_wifi_sta() {
    fprintf(stderr, "[esp32emu] esp_netif_create_default_wifi_sta\n");
    return &esp32emu::netif::staObj();
}

inline esp_netif_t esp_netif_create_default_wifi_ap() {
    fprintf(stderr, "[esp32emu] esp_netif_create_default_wifi_ap\n");
    return &esp32emu::netif::apObj();
}

inline esp_err_t esp_netif_get_ip_info(esp_netif_t netif, esp_netif_ip_info_t* ip_info) {
    if (!netif || !ip_info) return ESP_FAIL;
    auto* obj = netif;
    ip_info->ip.addr = ((uint32_t)obj->ip[0]) | ((uint32_t)obj->ip[1] << 8) |
                       ((uint32_t)obj->ip[2] << 16) | ((uint32_t)obj->ip[3] << 24);
    ip_info->gw.addr = ((uint32_t)obj->gw[0]) | ((uint32_t)obj->gw[1] << 8) |
                       ((uint32_t)obj->gw[2] << 16) | ((uint32_t)obj->gw[3] << 24);
    ip_info->netmask.addr = ((uint32_t)obj->netmask[0]) | ((uint32_t)obj->netmask[1] << 8) |
                            ((uint32_t)obj->netmask[2] << 16) | ((uint32_t)obj->netmask[3] << 24);
    return ESP_OK;
}

inline const char* esp_netif_get_ifkey(esp_netif_t netif) {
    return netif->is_sta ? "WIFI_STA_DEF" : "WIFI_AP_DEF";
}

// Helper to extract IP as string
inline void esp_netif_ip_to_str(uint32_t addr, char* buf, size_t len) {
    snprintf(buf, len, "%d.%d.%d.%d",
        (int)(addr & 0xFF), (int)((addr >> 8) & 0xFF),
        (int)((addr >> 16) & 0xFF), (int)((addr >> 24) & 0xFF));
}
