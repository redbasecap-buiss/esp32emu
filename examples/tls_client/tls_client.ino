#include <Arduino.h>
#include <mbedtls/ssl.h>

void setup() {
    Serial.begin(115200);

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);

    mbedtls_ssl_config_defaults(&conf,
        MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT);

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_setup(&ssl, &conf);
    mbedtls_ssl_set_hostname(&ssl, "example.com");

    int ret = mbedtls_ssl_handshake(&ssl);
    if (ret == 0) {
        Serial.print("Connected! Cipher: ");
        Serial.println(mbedtls_ssl_get_ciphersuite(&ssl));
    }

    mbedtls_ssl_close_notify(&ssl);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);

    Serial.println("TLS example done");
}

void loop() {
    delay(1000);
}
