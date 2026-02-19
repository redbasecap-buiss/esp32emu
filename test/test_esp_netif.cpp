// esp32emu test — ESP-IDF esp_netif API
#include <cassert>
#include <cstdio>
#include <cstring>
#include "esp_netif.h"

static void test_init() {
    esp32emu::netif::reset();
    assert(esp_netif_init() == ESP_OK);
    assert(esp32emu::netif::initialized());
    assert(esp_netif_deinit() == ESP_OK);
    assert(!esp32emu::netif::initialized());
}

static void test_create_sta() {
    esp32emu::netif::reset();
    esp_netif_init();
    esp_netif_t sta = esp_netif_create_default_wifi_sta();
    assert(sta != nullptr);
    assert(strcmp(esp_netif_get_ifkey(sta), "WIFI_STA_DEF") == 0);
    esp_netif_deinit();
}

static void test_create_ap() {
    esp32emu::netif::reset();
    esp_netif_init();
    esp_netif_t ap = esp_netif_create_default_wifi_ap();
    assert(ap != nullptr);
    assert(strcmp(esp_netif_get_ifkey(ap), "WIFI_AP_DEF") == 0);
    esp_netif_deinit();
}

static void test_get_ip_info() {
    esp32emu::netif::reset();
    esp_netif_init();
    esp_netif_t sta = esp_netif_create_default_wifi_sta();

    esp_netif_ip_info_t info;
    assert(esp_netif_get_ip_info(sta, &info) == ESP_OK);

    char ip_str[16];
    esp_netif_ip_to_str(info.ip.addr, ip_str, sizeof(ip_str));
    assert(strcmp(ip_str, "192.168.4.100") == 0);

    char gw_str[16];
    esp_netif_ip_to_str(info.gw.addr, gw_str, sizeof(gw_str));
    assert(strcmp(gw_str, "192.168.4.1") == 0);

    esp_netif_deinit();
}

static void test_set_sta_ip() {
    esp32emu::netif::reset();
    esp_netif_init();
    esp32emu::netif::setStaIp(10, 0, 0, 42);
    esp_netif_t sta = esp_netif_create_default_wifi_sta();

    esp_netif_ip_info_t info;
    esp_netif_get_ip_info(sta, &info);

    char ip_str[16];
    esp_netif_ip_to_str(info.ip.addr, ip_str, sizeof(ip_str));
    assert(strcmp(ip_str, "10.0.0.42") == 0);

    esp_netif_deinit();
}

static void test_null_args() {
    assert(esp_netif_get_ip_info(nullptr, nullptr) == ESP_FAIL);
}

int main() {
    test_init();
    test_create_sta();
    test_create_ap();
    test_get_ip_info();
    test_set_sta_ip();
    test_null_args();
    printf("test_esp_netif: all assertions passed\n");
    return 0;
}
