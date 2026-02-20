// test_esp_tls.cpp — Tests for esp_tls.h
#include "esp_tls.h"
#include <cassert>
#include <cstdio>

int main() {
    // Init
    esp_tls_t* tls = esp_tls_init();
    assert(tls != nullptr);
    assert(!tls->is_connected);
    assert(tls->sockfd == -1);
    
    // Connect
    esp_tls_cfg_t cfg = {};
    int ret = esp_tls_conn_new_sync("example.com", 11, 443, &cfg, tls);
    assert(ret == 1);
    assert(tls->is_connected);
    
    // Write
    const char* data = "GET / HTTP/1.1\r\n";
    ssize_t written = esp_tls_conn_write(tls, data, strlen(data));
    assert(written == (ssize_t)strlen(data));
    assert(tls->bytes_written == strlen(data));
    
    // Read (mock returns EOF)
    char buf[64];
    ssize_t rd = esp_tls_conn_read(tls, buf, sizeof(buf));
    assert(rd == 0);
    
    // Destroy
    assert(esp_tls_conn_destroy(tls) == 0);
    
    // Convenience: conn_new
    esp_tls_t* tls2 = esp_tls_conn_new("test.com", 8, 443, &cfg);
    assert(tls2 != nullptr);
    assert(tls2->is_connected);
    esp_tls_conn_destroy(tls2);
    
    // HTTP convenience
    esp_tls_t* tls3 = esp_tls_conn_http_new("https://example.com", &cfg);
    assert(tls3 != nullptr);
    assert(tls3->is_connected);
    esp_tls_conn_destroy(tls3);
    
    // Error handling
    int code = -1, flags = -1;
    assert(esp_tls_get_and_clear_last_error(nullptr, &code, &flags) == ESP_OK);
    assert(code == 0 && flags == 0);
    
    // Global CA store
    assert(esp_tls_global_ca_store_set(nullptr, 0) == ESP_OK);
    esp_tls_global_ca_store_free();
    
    // Write/read on null
    assert(esp_tls_conn_write(nullptr, "x", 1) == -1);
    assert(esp_tls_conn_read(nullptr, buf, 1) == -1);
    
    printf("test_esp_tls: all assertions passed\n");
    return 0;
}
