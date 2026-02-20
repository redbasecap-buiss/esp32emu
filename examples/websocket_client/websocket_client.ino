// ESP32 WebSocket Client Example
// Connects to a WebSocket server and exchanges messages.

#include <Arduino.h>
#include "esp_websocket_client.h"

static bool ws_connected = false;

static void websocket_event_handler(void *args, const char *base,
                                     int32_t event_id, void *event_data) {
    (void)args; (void)base;
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;

    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        Serial.println("[WS] Connected!");
        ws_connected = true;
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        Serial.println("[WS] Disconnected");
        ws_connected = false;
        break;
    case WEBSOCKET_EVENT_DATA:
        if (data->op_code == 1) { // text
            Serial.printf("[WS] Received: %.*s\n", data->data_len, data->data_ptr);
        } else {
            Serial.printf("[WS] Received binary (%d bytes)\n", data->data_len);
        }
        break;
    case WEBSOCKET_EVENT_CLOSED:
        Serial.println("[WS] Connection closed");
        ws_connected = false;
        break;
    default:
        break;
    }
}

static esp_websocket_client_handle_t client = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println("WebSocket Client Example");

    esp_websocket_client_config_t ws_cfg = {};
    ws_cfg.uri = "ws://echo.websocket.org";

    client = esp_websocket_client_init(&ws_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY,
                                   websocket_event_handler, nullptr);
    esp_websocket_client_start(client);
}

static int msg_count = 0;

void loop() {
    if (ws_connected && msg_count < 5) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Hello #%d from ESP32!", msg_count);
        esp_websocket_client_send_text(client, buf, strlen(buf), 1000);
        Serial.printf("[WS] Sent: %s\n", buf);
        msg_count++;
    }

    if (msg_count >= 5 && ws_connected) {
        Serial.println("[WS] Done, closing...");
        esp_websocket_client_close(client, 5000);
        esp_websocket_client_destroy(client);
        client = nullptr;
    }

    delay(1000);
}
