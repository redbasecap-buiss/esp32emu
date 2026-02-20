// https_server.cpp — ESP-IDF HTTPS server example
// Demonstrates setting up an HTTPS server (uses plain HTTP in emulation)

#include <Arduino.h>
#include "esp_https_server.h"

static httpd_handle_t server = nullptr;

static esp_err_t root_handler(httpd_req_t* req) {
    const char* resp = "{\"status\":\"ok\",\"tls\":true}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, strlen(resp));
    return ESP_OK;
}

static esp_err_t health_handler(httpd_req_t* req) {
    httpd_resp_send(req, "healthy", 7);
    return ESP_OK;
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF HTTPS Server Example");

    httpd_ssl_config_t cfg = HTTPD_SSL_CONFIG_DEFAULT();
    cfg.httpd.server_port = 8443;

    // In production you'd set cert/key:
    // cfg.servercert = server_cert_pem;
    // cfg.servercert_len = sizeof(server_cert_pem);
    // cfg.prvtkey_pem = server_key_pem;
    // cfg.prvtkey_len = sizeof(server_key_pem);

    esp_err_t ret = httpd_ssl_start(&server, &cfg);
    if (ret != ESP_OK) {
        Serial.printf("❌ Failed to start HTTPS server: %d\n", ret);
        return;
    }

    // Register handlers
    httpd_uri_t root = { .uri = "/", .method = HTTP_GET, .handler = root_handler, .user_ctx = nullptr };
    httpd_uri_t health = { .uri = "/health", .method = HTTP_GET, .handler = health_handler, .user_ctx = nullptr };
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &health);

    Serial.println("🔒 HTTPS server started on port 8443");
    Serial.println("   curl https://localhost:8443/");
    Serial.println("   curl https://localhost:8443/health");
}

void loop() {
    delay(1000);
}
