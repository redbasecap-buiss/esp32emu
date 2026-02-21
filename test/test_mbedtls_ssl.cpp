#include "mbedtls/ssl.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);

    // Configure as client
    assert(mbedtls_ssl_config_defaults(&conf,
        MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT) == 0);

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    assert(conf.authmode == MBEDTLS_SSL_VERIFY_REQUIRED);

    assert(mbedtls_ssl_setup(&ssl, &conf) == 0);
    assert(ssl._configured);
    assert(ssl.endpoint == MBEDTLS_SSL_IS_CLIENT);

    // Hostname
    assert(mbedtls_ssl_set_hostname(&ssl, "example.com") == 0);

    // Handshake
    assert(mbedtls_ssl_handshake(&ssl) == 0);

    // Write
    const unsigned char msg[] = "Hello";
    assert(mbedtls_ssl_write(&ssl, msg, 5) == 5);

    // Read returns WANT_READ (no data)
    unsigned char buf[64];
    assert(mbedtls_ssl_read(&ssl, buf, sizeof(buf)) == MBEDTLS_ERR_SSL_WANT_READ);

    // Ciphersuite
    const char *cs = mbedtls_ssl_get_ciphersuite(&ssl);
    assert(cs != nullptr);
    assert(strlen(cs) > 0);

    // Close
    assert(mbedtls_ssl_close_notify(&ssl) == 0);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);

    printf("test_mbedtls_ssl: all assertions passed\n");
    return 0;
}
