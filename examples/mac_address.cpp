// mac_address.cpp — ESP-IDF MAC address example
#include <Arduino.h>
#include "esp_mac.h"

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF MAC Address Example");
    Serial.println("===========================");

    uint8_t mac[6];

    // Base MAC
    esp_base_mac_addr_get(mac);
    Serial.printf("  Base MAC:     %02X:%02X:%02X:%02X:%02X:%02X\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // WiFi STA
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    Serial.printf("  WiFi STA MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // WiFi SoftAP
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    Serial.printf("  WiFi AP MAC:  %02X:%02X:%02X:%02X:%02X:%02X\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Bluetooth
    esp_read_mac(mac, ESP_MAC_BT);
    Serial.printf("  BT MAC:       %02X:%02X:%02X:%02X:%02X:%02X\n",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Derive local MAC
    uint8_t local[6];
    esp_derive_local_mac(local, mac);
    Serial.printf("  Local MAC:    %02X:%02X:%02X:%02X:%02X:%02X\n",
                  local[0], local[1], local[2], local[3], local[4], local[5]);
}

void loop() {
    delay(5000);
}
