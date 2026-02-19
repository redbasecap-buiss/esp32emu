// esp32emu test — ESP-IDF WiFi API
#include <cassert>
#include <cstdio>
#include <cstring>
#include "esp_wifi.h"

static void test_init_deinit() {
    esp32emu::wifi_idf::reset();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    assert(esp_wifi_init(&cfg) == ESP_OK);
    assert(esp_wifi_init(&cfg) == ESP_OK); // idempotent
    assert(esp_wifi_deinit() == ESP_OK);
}

static void test_mode() {
    esp32emu::wifi_idf::reset();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    assert(esp_wifi_set_mode(WIFI_MODE_STA) == ESP_OK);
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    assert(mode == WIFI_MODE_STA);

    assert(esp_wifi_set_mode(WIFI_MODE_AP) == ESP_OK);
    esp_wifi_get_mode(&mode);
    assert(mode == WIFI_MODE_AP);

    esp_wifi_deinit();
}

static void test_not_init_errors() {
    esp32emu::wifi_idf::reset();
    assert(esp_wifi_set_mode(WIFI_MODE_STA) == ESP_ERR_WIFI_NOT_INIT);
    assert(esp_wifi_start() == ESP_ERR_WIFI_NOT_INIT);
}

static void test_connect_flow() {
    esp32emu::wifi_idf::reset();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifi_cfg{};
    strcpy((char*)wifi_cfg.sta.ssid, "TestAP");
    strcpy((char*)wifi_cfg.sta.password, "pass1234");
    assert(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg) == ESP_OK);

    // Can't connect before start
    assert(esp_wifi_connect() == ESP_ERR_WIFI_NOT_STARTED);

    assert(esp_wifi_start() == ESP_OK);
    assert(esp32emu::wifi_idf::isStarted());

    assert(esp_wifi_connect() == ESP_OK);
    assert(esp32emu::wifi_idf::isConnected());

    assert(esp_wifi_disconnect() == ESP_OK);
    assert(!esp32emu::wifi_idf::isConnected());

    assert(esp_wifi_stop() == ESP_OK);
    assert(!esp32emu::wifi_idf::isStarted());

    esp_wifi_deinit();
}

static void test_config_roundtrip() {
    esp32emu::wifi_idf::reset();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t set_cfg{};
    strcpy((char*)set_cfg.sta.ssid, "MyNetwork");
    strcpy((char*)set_cfg.sta.password, "secret");
    esp_wifi_set_config(WIFI_IF_STA, &set_cfg);

    wifi_config_t get_cfg{};
    esp_wifi_get_config(WIFI_IF_STA, &get_cfg);
    assert(strcmp((char*)get_cfg.sta.ssid, "MyNetwork") == 0);
    assert(strcmp((char*)get_cfg.sta.password, "secret") == 0);

    esp_wifi_deinit();
}

static void test_scan() {
    esp32emu::wifi_idf::reset();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    esp32emu::wifi_idf::addScanResult("Network1", -45);
    esp32emu::wifi_idf::addScanResult("Network2", -70, WIFI_AUTH_OPEN);
    esp32emu::wifi_idf::addScanResult("Network3", -80);

    esp_wifi_scan_start(nullptr, true);

    uint16_t num = 0;
    esp_wifi_scan_get_ap_num(&num);
    assert(num == 3);

    wifi_ap_record_t records[3];
    uint16_t count = 3;
    esp_wifi_scan_get_ap_records(&count, records);
    assert(count == 3);
    assert(strcmp((char*)records[0].ssid, "Network1") == 0);
    assert(records[0].rssi == -45);
    assert(records[1].authmode == WIFI_AUTH_OPEN);

    esp_wifi_stop();
    esp_wifi_deinit();
}

static void test_ap_config() {
    esp32emu::wifi_idf::reset();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_AP);

    wifi_config_t ap_cfg{};
    strcpy((char*)ap_cfg.ap.ssid, "ESP32-AP");
    strcpy((char*)ap_cfg.ap.password, "appass");
    ap_cfg.ap.channel = 6;
    ap_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap_cfg.ap.max_connection = 4;
    esp_wifi_set_config(WIFI_IF_AP, &ap_cfg);

    wifi_config_t get_cfg{};
    esp_wifi_get_config(WIFI_IF_AP, &get_cfg);
    assert(strcmp((char*)get_cfg.ap.ssid, "ESP32-AP") == 0);
    assert(get_cfg.ap.channel == 6);
    assert(get_cfg.ap.max_connection == 4);

    esp_wifi_deinit();
}

int main() {
    test_init_deinit();
    test_mode();
    test_not_init_errors();
    test_connect_flow();
    test_config_roundtrip();
    test_scan();
    test_ap_config();
    printf("test_esp_wifi: all assertions passed\n");
    return 0;
}
