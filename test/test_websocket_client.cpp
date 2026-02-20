// Test: esp_websocket_client mock
#include "esp_websocket_client.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

static std::vector<int> events_received;
static std::string last_rx_data;

static void ws_handler(void *args, const char *base, int32_t event_id, void *event_data) {
    (void)args; (void)base;
    events_received.push_back(event_id);
    if (event_id == WEBSOCKET_EVENT_DATA) {
        auto *ev = (esp_websocket_event_data_t *)event_data;
        last_rx_data = std::string(ev->data_ptr, ev->data_len);
    }
}

int main() {
    esp_websocket_test_clear();
    events_received.clear();
    last_rx_data.clear();

    // Init
    esp_websocket_client_config_t cfg{};
    cfg.uri = "ws://localhost:8080/ws";
    auto client = esp_websocket_client_init(&cfg);
    assert(client != nullptr);

    // Register events
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, ws_handler, nullptr);

    // Start → CONNECTED event
    assert(esp_websocket_client_start(client) == 0);
    assert(esp_websocket_client_is_connected(client));
    assert(events_received.size() == 1);
    assert(events_received[0] == WEBSOCKET_EVENT_CONNECTED);

    // Send text
    int sent = esp_websocket_client_send_text(client, "hello", 5, 1000);
    assert(sent == 5);
    auto &log = esp_websocket_test_get_sent();
    assert(log.size() == 1);
    assert(log[0].data == "hello");
    assert(log[0].opcode == 1);

    // Send binary
    const char bin[] = {0x01, 0x02, 0x03};
    sent = esp_websocket_client_send_bin(client, bin, 3, 1000);
    assert(sent == 3);
    assert(log.size() == 2);
    assert(log[1].opcode == 2);
    assert(log[1].data.size() == 3);

    // Send text with auto-length (len=0)
    sent = esp_websocket_client_send_text(client, "world", 0, 1000);
    assert(sent == 5);
    assert(log.size() == 3);
    assert(log[2].data == "world");

    // Receive via inject + deliver
    client->inject_data("response_msg", 1);
    client->inject_data("binary_resp", 2);
    esp_websocket_test_deliver(client);
    // Should have CONNECTED + 2x DATA
    assert(events_received.size() == 3);
    assert(events_received[1] == WEBSOCKET_EVENT_DATA);
    assert(events_received[2] == WEBSOCKET_EVENT_DATA);
    assert(last_rx_data == "binary_resp"); // last received

    // Close
    events_received.clear();
    assert(esp_websocket_client_close(client, 1000) == 0);
    assert(!esp_websocket_client_is_connected(client));
    assert(events_received.size() == 1);
    assert(events_received[0] == WEBSOCKET_EVENT_CLOSED);

    // Send while disconnected fails
    assert(esp_websocket_client_send_text(client, "fail", 4, 100) == -1);

    // Destroy
    assert(esp_websocket_client_destroy(client) == 0);

    // Stop path (different from close)
    esp_websocket_test_clear();
    events_received.clear();
    cfg.uri = "ws://example.com/test";
    auto c2 = esp_websocket_client_init(&cfg);
    esp_websocket_register_events(c2, WEBSOCKET_EVENT_ANY, ws_handler, nullptr);
    esp_websocket_client_start(c2);
    assert(esp_websocket_client_is_connected(c2));
    events_received.clear();
    esp_websocket_client_stop(c2);
    assert(!esp_websocket_client_is_connected(c2));
    assert(events_received.size() == 1);
    assert(events_received[0] == WEBSOCKET_EVENT_DISCONNECTED);
    esp_websocket_client_destroy(c2);

    printf("test_websocket_client: all assertions passed\n");
    return 0;
}
