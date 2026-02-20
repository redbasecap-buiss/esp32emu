#pragma once
/*  esp_https_server.h — ESP-IDF HTTPS server mock for esp32emu
 *
 *  Wraps the existing esp_http_server mock with TLS config stubs.
 *  The actual transport is plain HTTP (no TLS in emulation).
 */

#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_tls.h"

typedef enum {
    HTTPD_SSL_TRANSPORT_SECURE = 0,
    HTTPD_SSL_TRANSPORT_INSECURE,
} httpd_ssl_transport_mode_t;

typedef struct httpd_ssl_config {
    httpd_config_t httpd;
    const uint8_t* cacert_pem;
    uint32_t cacert_len;
    const uint8_t* servercert;
    uint32_t servercert_len;
    const uint8_t* prvtkey_pem;
    uint32_t prvtkey_len;
    httpd_ssl_transport_mode_t transport_mode;
    uint16_t port_secure;
    uint16_t port_insecure;
} httpd_ssl_config_t;

inline httpd_ssl_config_t HTTPD_SSL_CONFIG_DEFAULT() {
    httpd_ssl_config_t cfg{};
    cfg.httpd = HTTPD_DEFAULT_CONFIG();
    cfg.cacert_pem = nullptr;
    cfg.cacert_len = 0;
    cfg.servercert = nullptr;
    cfg.servercert_len = 0;
    cfg.prvtkey_pem = nullptr;
    cfg.prvtkey_len = 0;
    cfg.transport_mode = HTTPD_SSL_TRANSPORT_SECURE;
    cfg.port_secure = 443;
    cfg.port_insecure = 80;
    return cfg;
}

/// Start HTTPS server — in emulation, delegates to plain httpd_start
inline esp_err_t httpd_ssl_start(httpd_handle_t* handle, httpd_ssl_config_t* config) {
    if (!handle || !config) return ESP_ERR_INVALID_ARG;
    return httpd_start(handle, &config->httpd);
}

/// Stop HTTPS server
inline esp_err_t httpd_ssl_stop(httpd_handle_t handle) {
    return httpd_stop(handle);
}
