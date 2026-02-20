#pragma once
// esp32emu: ESP-IDF WebSocket client mock
// Supports connect, send text/binary, receive via event handler, disconnect.

#include <cstdint>
#include <cstring>
#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

// ── Event types ─────────────────────────────────────────────────────
typedef enum {
    WEBSOCKET_EVENT_ANY = -1,
    WEBSOCKET_EVENT_ERROR = 0,
    WEBSOCKET_EVENT_CONNECTED,
    WEBSOCKET_EVENT_DISCONNECTED,
    WEBSOCKET_EVENT_DATA,
    WEBSOCKET_EVENT_CLOSED,
    WEBSOCKET_EVENT_MAX
} esp_websocket_event_id_t;

// ── Event data ──────────────────────────────────────────────────────
typedef struct {
    const char *data_ptr;
    int data_len;
    int op_code;          // 1=text, 2=binary, 8=close, 9=ping, 10=pong
    int payload_len;
    int payload_offset;
} esp_websocket_event_data_t;

// ── Config ──────────────────────────────────────────────────────────
typedef struct {
    const char *uri;
    const char *host;
    int port;
    const char *path;
    const char *username;
    const char *password;
    const char *subprotocol;
    const char *user_agent;
    const char *headers;
    int buffer_size;
    int task_stack;
    int task_prio;
    bool disable_auto_reconnect;
    int reconnect_timeout_ms;
    int network_timeout_ms;
    int ping_interval_sec;
    bool disable_pingpong_discon;
} esp_websocket_client_config_t;

// ── Handle ──────────────────────────────────────────────────────────
struct esp_websocket_client;
typedef struct esp_websocket_client *esp_websocket_client_handle_t;

typedef void (*esp_websocket_event_handler_t)(void *handler_args,
                                              const char *base,
                                              int32_t event_id,
                                              void *event_data);

#ifdef __cplusplus
}
#endif

// ── Implementation (header-only for esp32emu) ───────────────────────
#ifdef __cplusplus

struct esp_websocket_client {
    std::string uri;
    bool connected = false;
    bool started = false;
    esp_websocket_event_handler_t handler = nullptr;
    void *handler_args = nullptr;
    std::vector<std::pair<int, std::string>> rx_queue; // (opcode, data) injected by tests
    std::mutex mtx;

    // Test helper: inject a received message
    void inject_data(const std::string &data, int op_code = 1) {
        std::lock_guard<std::mutex> lk(mtx);
        rx_queue.push_back({op_code, data});
    }

    void fire_event(esp_websocket_event_id_t id, const char *data = nullptr, int len = 0, int op = 1) {
        if (!handler) return;
        esp_websocket_event_data_t ev{};
        ev.data_ptr = data;
        ev.data_len = len;
        ev.payload_len = len;
        ev.payload_offset = 0;
        ev.op_code = op;
        handler(handler_args, "websocket", (int32_t)id, &ev);
    }
};

// ── Sent message log (for test verification) ────────────────────────
struct esp_websocket_sent_msg {
    std::string data;
    int opcode;
};

inline std::vector<esp_websocket_sent_msg> &__esp_ws_sent_log() {
    static std::vector<esp_websocket_sent_msg> log;
    return log;
}

inline void esp_websocket_test_clear() {
    __esp_ws_sent_log().clear();
}

inline const std::vector<esp_websocket_sent_msg> &esp_websocket_test_get_sent() {
    return __esp_ws_sent_log();
}

// ── API ─────────────────────────────────────────────────────────────
inline esp_websocket_client_handle_t esp_websocket_client_init(const esp_websocket_client_config_t *config) {
    auto *c = new esp_websocket_client();
    if (config->uri) c->uri = config->uri;
    return c;
}

inline int esp_websocket_register_events(esp_websocket_client_handle_t client,
                                          esp_websocket_event_id_t event,
                                          esp_websocket_event_handler_t handler,
                                          void *handler_args) {
    (void)event; // register for all
    client->handler = handler;
    client->handler_args = handler_args;
    return 0; // ESP_OK
}

inline int esp_websocket_client_start(esp_websocket_client_handle_t client) {
    client->started = true;
    client->connected = true;
    client->fire_event(WEBSOCKET_EVENT_CONNECTED);
    return 0;
}

inline int esp_websocket_client_stop(esp_websocket_client_handle_t client) {
    if (client->connected) {
        client->connected = false;
        client->fire_event(WEBSOCKET_EVENT_DISCONNECTED);
    }
    client->started = false;
    return 0;
}

inline bool esp_websocket_client_is_connected(esp_websocket_client_handle_t client) {
    return client->connected;
}

inline int esp_websocket_client_send_text(esp_websocket_client_handle_t client,
                                           const char *data, int len, int timeout_ms) {
    (void)timeout_ms;
    if (!client->connected) return -1;
    int actual_len = (len <= 0) ? (int)strlen(data) : len;
    __esp_ws_sent_log().push_back({std::string(data, actual_len), 1});
    return actual_len;
}

inline int esp_websocket_client_send_bin(esp_websocket_client_handle_t client,
                                          const char *data, int len, int timeout_ms) {
    (void)timeout_ms;
    if (!client->connected) return -1;
    __esp_ws_sent_log().push_back({std::string(data, len), 2});
    return len;
}

// Test helper: deliver all queued rx messages as DATA events
inline void esp_websocket_test_deliver(esp_websocket_client_handle_t client) {
    std::lock_guard<std::mutex> lk(client->mtx);
    for (auto &[op, data] : client->rx_queue) {
        client->fire_event(WEBSOCKET_EVENT_DATA, data.c_str(), (int)data.size(), op);
    }
    client->rx_queue.clear();
}

inline int esp_websocket_client_close(esp_websocket_client_handle_t client, int timeout_ms) {
    (void)timeout_ms;
    if (client->connected) {
        client->connected = false;
        client->fire_event(WEBSOCKET_EVENT_CLOSED);
    }
    return 0;
}

inline int esp_websocket_client_destroy(esp_websocket_client_handle_t client) {
    delete client;
    return 0;
}

#endif // __cplusplus
