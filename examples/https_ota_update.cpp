// HTTPS OTA firmware update example
#include <Arduino.h>
#include <WiFi.h>
#include "esp_https_ota.h"

#define FIRMWARE_URL "https://example.com/firmware.bin"

void performOTA() {
    Serial.println("Starting HTTPS OTA update...");

    esp_http_client_config_t http_cfg = {};
    http_cfg.url = FIRMWARE_URL;

    esp_https_ota_config_t ota_cfg = {};
    ota_cfg.http_config = http_cfg;

    esp_https_ota_handle_t handle = nullptr;
    esp_err_t err = esp_https_ota_begin(&ota_cfg, &handle);
    if (err != ESP_OK) {
        Serial.println("OTA begin failed!");
        return;
    }

    while (true) {
        err = esp_https_ota_perform(handle);
        if (err == ESP_OK) break;
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            Serial.println("OTA perform failed!");
            return;
        }
        int read = esp_https_ota_get_image_len_read(handle);
        int total = esp_https_ota_get_image_size(handle);
        Serial.printf("Progress: %d / %d bytes (%d%%)\n", read, total, read * 100 / total);
    }

    if (esp_https_ota_finish(handle) == ESP_OK) {
        Serial.println("OTA update successful! Restarting...");
        ESP.restart();
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("MyNetwork", "password");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println("WiFi connected");
    performOTA();
}

void loop() {
    delay(1000);
}
