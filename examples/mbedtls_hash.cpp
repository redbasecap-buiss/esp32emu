// esp32emu example — SHA-256 hashing and Base64 encoding with mbedtls
#include <Arduino.h>
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"

void printHex(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        char buf[4];
        snprintf(buf, sizeof(buf), "%02x", data[i]);
        Serial.print(buf);
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    Serial.println("mbedTLS Hash & Base64 Example");
    Serial.println("=============================");

    // SHA-256 hash
    const char* message = "Hello from ESP32!";
    uint8_t hash[32];
    mbedtls_sha256((const unsigned char*)message, strlen(message), hash, 0);
    Serial.print("SHA-256(\"");
    Serial.print(message);
    Serial.print("\"): ");
    printHex(hash, 32);

    // Streaming hash
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, (const unsigned char*)"Hello ", 6);
    mbedtls_sha256_update(&ctx, (const unsigned char*)"from ESP32!", 11);
    uint8_t hash2[32];
    mbedtls_sha256_finish(&ctx, hash2);
    mbedtls_sha256_free(&ctx);
    Serial.print("Streaming:  ");
    printHex(hash2, 32);
    Serial.print("Match: ");
    Serial.println(memcmp(hash, hash2, 32) == 0 ? "YES" : "NO");

    // Base64 encode the hash
    unsigned char b64[128];
    size_t olen;
    mbedtls_base64_encode(b64, sizeof(b64), &olen, hash, 32);
    Serial.print("Base64: ");
    Serial.println((char*)b64);

    // Base64 roundtrip
    unsigned char decoded[64];
    size_t dlen;
    mbedtls_base64_decode(decoded, sizeof(decoded), &dlen, b64, olen);
    Serial.print("Decoded matches: ");
    Serial.println(memcmp(decoded, hash, 32) == 0 ? "YES" : "NO");

    // HMAC
    const char* key = "my-secret-key";
    uint8_t hmac[32];
    const mbedtls_md_info_t* info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    mbedtls_md_hmac(info, (const unsigned char*)key, strlen(key),
                     (const unsigned char*)message, strlen(message), hmac);
    Serial.print("HMAC-SHA256: ");
    printHex(hmac, 32);
}

void loop() {
    delay(5000);
}
