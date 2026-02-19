// esp32emu example — ESP-IDF WiFi Station (classic pattern)
// Shows the standard esp_netif + esp_wifi + event groups pattern.
#include <cstdio>
#include <cstring>
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char* TAG = "wifi_station";

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t s_wifi_event_group;

static void wifi_init_sta() {
    s_wifi_event_group = xEventGroupCreate();

    // Initialize networking
    esp_netif_init();
    esp_netif_t sta_netif = esp_netif_create_default_wifi_sta();

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // Configure STA
    wifi_config_t wifi_config{};
    strcpy((char*)wifi_config.sta.ssid, "MyHomeNetwork");
    strcpy((char*)wifi_config.sta.password, "supersecret");
    wifi_config.sta.threshold_authmode = WIFI_AUTH_WPA2_PSK;

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    ESP_LOGI(TAG, "wifi_init_sta finished");

    // In real code this would be event-driven; here we just signal success
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

    // Wait for connection
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to SSID: %s", wifi_config.sta.ssid);

        // Print IP
        esp_netif_ip_info_t ip_info;
        esp_netif_get_ip_info(sta_netif, &ip_info);
        char ip_str[16];
        esp_netif_ip_to_str(ip_info.ip.addr, ip_str, sizeof(ip_str));
        ESP_LOGI(TAG, "IP Address: %s", ip_str);
    } else {
        ESP_LOGE(TAG, "Failed to connect");
    }

    vEventGroupDelete(s_wifi_event_group);
}

int main() {
    printf("=== ESP-IDF WiFi Station Example ===\n");
    wifi_init_sta();
    printf("=== Done ===\n");
    return 0;
}
