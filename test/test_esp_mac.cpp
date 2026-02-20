// test_esp_mac.cpp — Tests for esp_mac.h
#include "esp_mac.h"
#include <cassert>
#include <cstdio>

int main() {
    uint8_t mac[6] = {};
    
    // Base MAC
    assert(esp_base_mac_addr_get(mac) == ESP_OK);
    assert(mac[0] == 0xAA && mac[1] == 0xBB && mac[5] == 0x00);
    
    // Per-interface MAC
    assert(esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK);
    assert(mac[5] == 0x00);
    
    assert(esp_read_mac(mac, ESP_MAC_BT) == ESP_OK);
    assert(mac[5] == 0x02);
    
    assert(esp_read_mac(mac, ESP_MAC_ETH) == ESP_OK);
    assert(mac[5] == 0x03);
    
    // Derive local MAC
    uint8_t universal[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    uint8_t local[6] = {};
    assert(esp_derive_local_mac(local, universal) == ESP_OK);
    assert(local[0] == 0x02);  // locally administered bit set
    assert(local[1] == 0x11);
    
    // Null checks
    assert(esp_base_mac_addr_get(nullptr) == ESP_ERR_INVALID_ARG);
    assert(esp_read_mac(nullptr, ESP_MAC_WIFI_STA) == ESP_ERR_INVALID_ARG);
    assert(esp_derive_local_mac(nullptr, universal) == ESP_ERR_INVALID_ARG);
    
    // Set base MAC (no-op)
    assert(esp_base_mac_addr_set(mac) == ESP_OK);
    
    printf("test_esp_mac: all assertions passed\n");
    return 0;
}
