// ESP-IDF esp_http_client example
// Demonstrates the C-style HTTP client API with event handler.

#include <Arduino.h>
#include "esp_http_client.h"

static esp_err_t http_event_handler(esp_http_client_event_t* evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_CONNECTED:
            Serial.println("📡 Connected to server");
            break;
        case HTTP_EVENT_ON_DATA:
            Serial.printf("📦 Received %d bytes\n", evt->data_len);
            if (evt->data_len > 0) {
                Serial.printf("   Data: %.*s\n", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            Serial.println("✅ Request finished");
            break;
        default:
            break;
    }
    return ESP_OK;
}

void doGet() {
    Serial.println("\n--- GET Request ---");
    esp_http_client_config_t config = {};
    config.url = "http://httpbin.org/get";
    config.event_handler = http_event_handler;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        Serial.printf("Status: %d, Content-Length: %d\n",
                      esp_http_client_get_status_code(client),
                      esp_http_client_get_content_length(client));
    } else {
        Serial.printf("HTTP GET failed: %d\n", err);
    }
    esp_http_client_cleanup(client);
}

void doPost() {
    Serial.println("\n--- POST Request ---");
    esp_http_client_config_t config = {};
    config.url = "http://httpbin.org/post";
    config.method = HTTP_METHOD_POST;
    config.event_handler = http_event_handler;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    const char* post_data = "{\"sensor\":\"temp\",\"value\":23.5}";
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, (int)strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        Serial.printf("Status: %d\n", esp_http_client_get_status_code(client));
    }
    esp_http_client_cleanup(client);
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF HTTP Client Demo");

    doGet();
    doPost();
}

void loop() {
    delay(10000);
    doGet();
}
