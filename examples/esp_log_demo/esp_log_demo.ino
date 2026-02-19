// ESP-IDF logging demo
// Demonstrates ESP_LOGE/W/I/D/V macros and log level filtering

#include <Arduino.h>
#include <esp_log.h>

static const char* TAG = "main";
static const char* NET_TAG = "network";

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF Logging Demo");

    // Default level is INFO — DEBUG and VERBOSE won't show
    ESP_LOGE(TAG, "This is an ERROR message (always visible)");
    ESP_LOGW(TAG, "This is a WARNING message");
    ESP_LOGI(TAG, "This is an INFO message");
    ESP_LOGD(TAG, "This DEBUG message is hidden at INFO level");
    ESP_LOGV(TAG, "This VERBOSE message is also hidden");

    // Increase log level to see everything
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    ESP_LOGD(NET_TAG, "Connecting to WiFi...");
    ESP_LOGV(NET_TAG, "SSID scan complete, %d networks found", 5);
    ESP_LOGI(NET_TAG, "Connected! IP: 192.168.1.42");
}

void loop() {
    static int count = 0;
    ESP_LOGI(TAG, "Loop iteration %d", count++);
    delay(1000);
}
