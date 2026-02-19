// esp32emu example — FreeRTOS Event Groups for WiFi connection synchronization
// Demonstrates using event groups to coordinate tasks waiting for WiFi.
#include <Arduino.h>
#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static EventGroupHandle_t s_wifi_event_group;

void httpTask(void* param) {
    (void)param;
    Serial.println("[HTTP] Waiting for WiFi...");
    EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        Serial.println("[HTTP] WiFi connected! Starting HTTP requests...");
        Serial.print("[HTTP] IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("[HTTP] WiFi failed, cannot start HTTP.");
    }
    vTaskDelete(NULL);
}

void mqttTask(void* param) {
    (void)param;
    Serial.println("[MQTT] Waiting for WiFi...");
    EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        Serial.println("[MQTT] WiFi connected! Connecting to broker...");
    } else {
        Serial.println("[MQTT] WiFi failed, cannot connect.");
    }
    vTaskDelete(NULL);
}

void setup() {
    Serial.begin(115200);
    Serial.println("=== Event Groups WiFi Sync Example ===");

    s_wifi_event_group = xEventGroupCreate();

    // Start tasks that need WiFi — they'll block on the event group
    xTaskCreate(httpTask, "http", 4096, NULL, 1, NULL);
    xTaskCreate(mqttTask, "mqtt", 4096, NULL, 1, NULL);

    // Simulate WiFi connection
    delay(50);
    WiFi.begin("TestNetwork", "password123");
    delay(50);

    Serial.println("[WiFi] Connected!");
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    delay(100); // Let tasks run
}

void loop() {
    Serial.println("[Main] All tasks notified. Done.");
    delay(1000);
}
