#pragma once
/*  esp_tls.h — ESP-IDF TLS connection mock for esp32emu
 *
 *  Provides the esp_tls API surface used by esp_http_client and
 *  esp_https_server. Under the hood this is a thin wrapper around
 *  a POSIX TCP socket (no real TLS — this is a test mock).
 */

#include "esp_err.h"
#include <cstdint>
#include <cstddef>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_tls {
    int sockfd;
    bool is_connected;
    size_t bytes_written;
    size_t bytes_read;
} esp_tls_t;

typedef struct esp_tls_cfg {
    const char* cacert_pem_buf;
    unsigned int cacert_pem_bytes;
    const char* clientcert_pem_buf;
    unsigned int clientcert_pem_bytes;
    const char* clientkey_pem_buf;
    unsigned int clientkey_pem_bytes;
    bool non_block;
    int timeout_ms;
    bool use_global_ca_store;
    bool skip_common_name;
} esp_tls_cfg_t;

inline esp_tls_t* esp_tls_init() {
    auto* tls = new esp_tls_t{};
    tls->sockfd = -1;
    tls->is_connected = false;
    tls->bytes_written = 0;
    tls->bytes_read = 0;
    return tls;
}

inline int esp_tls_conn_new_sync(const char* /*hostname*/, int /*hostlen*/,
                                  int /*port*/, const esp_tls_cfg_t* /*cfg*/,
                                  esp_tls_t* tls) {
    if (!tls) return -1;
    // Mock: just mark as connected (no real TLS)
    tls->is_connected = true;
    tls->sockfd = 999; // fake fd
    return 1; // success
}

inline esp_tls_t* esp_tls_conn_new(const char* hostname, int hostlen,
                                    int port, const esp_tls_cfg_t* cfg) {
    esp_tls_t* tls = esp_tls_init();
    if (esp_tls_conn_new_sync(hostname, hostlen, port, cfg, tls) < 0) {
        delete tls;
        return nullptr;
    }
    return tls;
}

inline esp_tls_t* esp_tls_conn_http_new(const char* /*url*/, const esp_tls_cfg_t* /*cfg*/) {
    esp_tls_t* tls = esp_tls_init();
    tls->is_connected = true;
    tls->sockfd = 999;
    return tls;
}

inline ssize_t esp_tls_conn_write(esp_tls_t* tls, const void* data, size_t datalen) {
    if (!tls || !tls->is_connected) return -1;
    tls->bytes_written += datalen;
    (void)data;
    return (ssize_t)datalen;
}

inline ssize_t esp_tls_conn_read(esp_tls_t* tls, void* data, size_t datalen) {
    if (!tls || !tls->is_connected) return -1;
    // Mock: return 0 (EOF) — tests can override
    memset(data, 0, datalen);
    return 0;
}

inline int esp_tls_conn_destroy(esp_tls_t* tls) {
    if (tls) {
        tls->is_connected = false;
        delete tls;
    }
    return 0;
}

inline esp_err_t esp_tls_get_and_clear_last_error(esp_tls_t* /*tls*/, int* esp_tls_code, int* esp_tls_flags) {
    if (esp_tls_code) *esp_tls_code = 0;
    if (esp_tls_flags) *esp_tls_flags = 0;
    return ESP_OK;
}

inline esp_err_t esp_tls_global_ca_store_set(const unsigned char* /*cacert*/, unsigned int /*len*/) {
    return ESP_OK;
}

inline void esp_tls_global_ca_store_free() {}

#ifdef __cplusplus
}
#endif
