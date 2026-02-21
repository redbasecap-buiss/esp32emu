#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>

// Minimal mbedTLS SSL mock for ESP-IDF projects

#define MBEDTLS_SSL_IS_CLIENT 0
#define MBEDTLS_SSL_IS_SERVER 1
#define MBEDTLS_SSL_TRANSPORT_STREAM 0
#define MBEDTLS_SSL_TRANSPORT_DATAGRAM 1
#define MBEDTLS_SSL_PRESET_DEFAULT 0
#define MBEDTLS_SSL_VERIFY_NONE 0
#define MBEDTLS_SSL_VERIFY_OPTIONAL 1
#define MBEDTLS_SSL_VERIFY_REQUIRED 2

#define MBEDTLS_ERR_SSL_WANT_READ (-0x6900)
#define MBEDTLS_ERR_SSL_WANT_WRITE (-0x6880)
#define MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY (-0x7880)

typedef struct mbedtls_ssl_context {
    int endpoint;
    int transport;
    int authmode;
    bool _configured;
} mbedtls_ssl_context;

typedef struct mbedtls_ssl_config {
    int endpoint;
    int transport;
    int authmode;
    int preset;
} mbedtls_ssl_config;

inline void mbedtls_ssl_init(mbedtls_ssl_context *ssl) {
    memset(ssl, 0, sizeof(*ssl));
    printf("[esp32emu] mbedtls_ssl_init\n");
}

inline void mbedtls_ssl_config_init(mbedtls_ssl_config *conf) {
    memset(conf, 0, sizeof(*conf));
}

inline int mbedtls_ssl_config_defaults(mbedtls_ssl_config *conf,
                                        int endpoint, int transport, int preset) {
    conf->endpoint = endpoint;
    conf->transport = transport;
    conf->preset = preset;
    return 0;
}

inline void mbedtls_ssl_conf_authmode(mbedtls_ssl_config *conf, int authmode) {
    conf->authmode = authmode;
}

inline int mbedtls_ssl_setup(mbedtls_ssl_context *ssl, const mbedtls_ssl_config *conf) {
    ssl->endpoint = conf->endpoint;
    ssl->transport = conf->transport;
    ssl->authmode = conf->authmode;
    ssl->_configured = true;
    return 0;
}

inline int mbedtls_ssl_set_hostname(mbedtls_ssl_context *ssl, const char *hostname) {
    (void)ssl; (void)hostname;
    return 0;
}

inline int mbedtls_ssl_handshake(mbedtls_ssl_context *ssl) {
    (void)ssl;
    printf("[esp32emu] mbedtls_ssl_handshake: OK (mock)\n");
    return 0;
}

inline int mbedtls_ssl_write(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len) {
    (void)ssl; (void)buf;
    return (int)len;
}

inline int mbedtls_ssl_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len) {
    (void)ssl; (void)buf; (void)len;
    return MBEDTLS_ERR_SSL_WANT_READ;
}

inline int mbedtls_ssl_close_notify(mbedtls_ssl_context *ssl) {
    (void)ssl;
    return 0;
}

inline void mbedtls_ssl_free(mbedtls_ssl_context *ssl) {
    memset(ssl, 0, sizeof(*ssl));
}

inline void mbedtls_ssl_config_free(mbedtls_ssl_config *conf) {
    memset(conf, 0, sizeof(*conf));
}

inline const char *mbedtls_ssl_get_ciphersuite(const mbedtls_ssl_context *ssl) {
    (void)ssl;
    return "TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256";
}
