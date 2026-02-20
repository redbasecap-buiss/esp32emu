// test_esp_https_server.cpp — Tests for esp_https_server.h
#include "esp_https_server.h"
#include <cassert>
#include <cstdio>

int main() {
    // Config with defaults
    httpd_ssl_config_t ssl_cfg = HTTPD_SSL_CONFIG_DEFAULT();
    ssl_cfg.httpd.server_port = 0;  // auto-assign
    
    assert(ssl_cfg.transport_mode == HTTPD_SSL_TRANSPORT_SECURE);
    assert(ssl_cfg.port_secure == 443);
    assert(ssl_cfg.port_insecure == 80);
    
    // Start server (delegates to httpd_start)
    httpd_handle_t server = nullptr;
    assert(httpd_ssl_start(&server, &ssl_cfg) == ESP_OK);
    assert(server != nullptr);
    
    // Stop server
    assert(httpd_ssl_stop(server) == ESP_OK);
    
    // Error cases
    assert(httpd_ssl_start(nullptr, &ssl_cfg) == ESP_ERR_INVALID_ARG);
    assert(httpd_ssl_start(&server, nullptr) == ESP_ERR_INVALID_ARG);
    
    printf("test_esp_https_server: all assertions passed\n");
    return 0;
}
