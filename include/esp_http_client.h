#pragma once
// esp32emu: ESP-IDF esp_http_client mock
// Provides the ESP-IDF C-style HTTP client API backed by POSIX sockets.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <map>

#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif
#ifndef ESP_ERR_INVALID_ARG
#define ESP_ERR_INVALID_ARG 0x102
#endif

typedef int esp_err_t;

typedef enum {
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_PATCH,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_OPTIONS
} esp_http_client_method_t;

typedef enum {
    HTTP_EVENT_ERROR = 0,
    HTTP_EVENT_ON_CONNECTED,
    HTTP_EVENT_HEADER_SENT,
    HTTP_EVENT_ON_HEADER,
    HTTP_EVENT_ON_DATA,
    HTTP_EVENT_ON_FINISH,
    HTTP_EVENT_DISCONNECTED
} esp_http_client_event_id_t;

struct esp_http_client_event_t {
    esp_http_client_event_id_t event_id;
    void* client;
    void* data;
    int data_len;
    void* user_data;
    char* header_key;
    char* header_value;
};

typedef esp_err_t (*http_event_handle_cb)(esp_http_client_event_t* evt);

typedef struct {
    const char* url;
    const char* host;
    int port;
    const char* path;
    const char* username;
    const char* password;
    esp_http_client_method_t method;
    int timeout_ms;
    bool disable_auto_redirect;
    int max_redirection_count;
    http_event_handle_cb event_handler;
    void* user_data;
    const char* cert_pem;
    bool skip_cert_common_name_check;
} esp_http_client_config_t;

// ── Internal client struct ─────────────────────────────────────────
struct esp_http_client {
    std::string url;
    std::string host;
    std::string path;
    int port = 80;
    esp_http_client_method_t method = HTTP_METHOD_GET;
    int timeout_ms = 5000;
    http_event_handle_cb event_handler = nullptr;
    void* user_data = nullptr;

    // Request
    std::map<std::string, std::string> headers;
    std::string post_data;

    // Response (mock)
    int status_code = 200;
    std::string response_body;
    int content_length = 0;
    bool performed = false;
};

typedef esp_http_client* esp_http_client_handle_t;

// ── API ────────────────────────────────────────────────────────────
inline esp_http_client_handle_t esp_http_client_init(const esp_http_client_config_t* config) {
    if (!config) return nullptr;
    auto* c = new esp_http_client();
    if (config->url) c->url = config->url;
    if (config->host) c->host = config->host;
    if (config->path) c->path = config->path;
    c->port = config->port ? config->port : 80;
    c->method = config->method;
    c->timeout_ms = config->timeout_ms ? config->timeout_ms : 5000;
    c->event_handler = config->event_handler;
    c->user_data = config->user_data;
    printf("[esp_http_client] init: %s\n", c->url.c_str());
    return c;
}

inline esp_err_t esp_http_client_set_url(esp_http_client_handle_t client, const char* url) {
    if (!client || !url) return ESP_ERR_INVALID_ARG;
    client->url = url;
    return ESP_OK;
}

inline esp_err_t esp_http_client_set_method(esp_http_client_handle_t client,
                                             esp_http_client_method_t method) {
    if (!client) return ESP_ERR_INVALID_ARG;
    client->method = method;
    return ESP_OK;
}

inline esp_err_t esp_http_client_set_header(esp_http_client_handle_t client,
                                             const char* key, const char* value) {
    if (!client || !key || !value) return ESP_ERR_INVALID_ARG;
    client->headers[key] = value;
    return ESP_OK;
}

inline esp_err_t esp_http_client_set_post_field(esp_http_client_handle_t client,
                                                 const char* data, int len) {
    if (!client) return ESP_ERR_INVALID_ARG;
    client->post_data = std::string(data, (size_t)len);
    client->content_length = len;
    return ESP_OK;
}

// Mock: does not actually connect. Sets a 200 OK with empty body.
// Test code can inject response via client->response_body / status_code before calling.
inline esp_err_t esp_http_client_perform(esp_http_client_handle_t client) {
    if (!client) return ESP_ERR_INVALID_ARG;
    static const char* methods[] = {"GET","POST","PUT","PATCH","DELETE","HEAD","OPTIONS"};
    printf("[esp_http_client] %s %s\n", methods[(int)client->method], client->url.c_str());

    if (!client->performed) {
        // Default mock response
        if (client->response_body.empty()) {
            client->response_body = "{\"status\":\"ok\"}";
        }
        client->content_length = (int)client->response_body.size();
        client->status_code = 200;
    }
    client->performed = true;

    // Fire events if handler registered
    if (client->event_handler) {
        esp_http_client_event_t evt{};
        evt.client = client;
        evt.user_data = client->user_data;

        evt.event_id = HTTP_EVENT_ON_CONNECTED;
        client->event_handler(&evt);

        evt.event_id = HTTP_EVENT_ON_DATA;
        evt.data = (void*)client->response_body.data();
        evt.data_len = (int)client->response_body.size();
        client->event_handler(&evt);

        evt.event_id = HTTP_EVENT_ON_FINISH;
        evt.data = nullptr;
        evt.data_len = 0;
        client->event_handler(&evt);
    }

    return ESP_OK;
}

inline int esp_http_client_get_status_code(esp_http_client_handle_t client) {
    return client ? client->status_code : -1;
}

inline int esp_http_client_get_content_length(esp_http_client_handle_t client) {
    return client ? client->content_length : -1;
}

inline int esp_http_client_read(esp_http_client_handle_t client, char* buffer, int len) {
    if (!client || !buffer) return -1;
    int to_read = std::min(len, (int)client->response_body.size());
    memcpy(buffer, client->response_body.data(), (size_t)to_read);
    client->response_body.erase(0, (size_t)to_read);
    return to_read;
}

inline esp_err_t esp_http_client_cleanup(esp_http_client_handle_t client) {
    if (client) {
        printf("[esp_http_client] cleanup\n");
        delete client;
    }
    return ESP_OK;
}

inline esp_err_t esp_http_client_close(esp_http_client_handle_t client) {
    (void)client;
    return ESP_OK;
}
