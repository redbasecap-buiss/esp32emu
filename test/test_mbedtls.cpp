// esp32emu — mbedtls mock tests (sha256, md, base64)
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // SHA-256 one-shot: deterministic output
    uint8_t hash1[32], hash2[32];
    const char* msg = "Hello ESP32";
    mbedtls_sha256((const unsigned char*)msg, strlen(msg), hash1, 0);
    mbedtls_sha256((const unsigned char*)msg, strlen(msg), hash2, 0);
    assert(memcmp(hash1, hash2, 32) == 0);  // Same input -> same hash

    // Different input -> different hash
    const char* msg2 = "Hello ESP33";
    uint8_t hash3[32];
    mbedtls_sha256((const unsigned char*)msg2, strlen(msg2), hash3, 0);
    assert(memcmp(hash1, hash3, 32) != 0);

    // Streaming SHA-256
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, (const unsigned char*)"Hello ", 6);
    mbedtls_sha256_update(&ctx, (const unsigned char*)"ESP32", 5);
    uint8_t hash4[32];
    mbedtls_sha256_finish(&ctx, hash4);
    assert(memcmp(hash1, hash4, 32) == 0);  // Streaming == one-shot
    mbedtls_sha256_free(&ctx);

    // MD API
    const mbedtls_md_info_t* sha256_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    assert(sha256_info != nullptr);
    assert(mbedtls_md_get_size(sha256_info) == 32);

    uint8_t md_out[32];
    mbedtls_md(sha256_info, (const unsigned char*)msg, strlen(msg), md_out);
    assert(memcmp(hash1, md_out, 32) == 0);

    // HMAC
    uint8_t hmac1[32], hmac2[32];
    const char* key = "secret";
    mbedtls_md_hmac(sha256_info, (const unsigned char*)key, strlen(key),
                     (const unsigned char*)msg, strlen(msg), hmac1);
    mbedtls_md_hmac(sha256_info, (const unsigned char*)key, strlen(key),
                     (const unsigned char*)msg, strlen(msg), hmac2);
    assert(memcmp(hmac1, hmac2, 32) == 0);  // Deterministic

    // MD context streaming
    mbedtls_md_context_t md_ctx;
    mbedtls_md_init(&md_ctx);
    mbedtls_md_setup(&md_ctx, sha256_info, 0);
    mbedtls_md_starts(&md_ctx);
    mbedtls_md_update(&md_ctx, (const unsigned char*)msg, strlen(msg));
    uint8_t md_stream[32];
    mbedtls_md_finish(&md_ctx, md_stream);
    assert(memcmp(hash1, md_stream, 32) == 0);
    mbedtls_md_free(&md_ctx);

    // Base64 encode
    unsigned char encoded[64];
    size_t olen;
    int ret = mbedtls_base64_encode(encoded, sizeof(encoded), &olen,
                                     (const unsigned char*)"Hello", 5);
    assert(ret == 0);
    assert(strcmp((char*)encoded, "SGVsbG8=") == 0);

    // Base64 decode
    unsigned char decoded[64];
    size_t dlen;
    ret = mbedtls_base64_decode(decoded, sizeof(decoded), &dlen, encoded, olen);
    assert(ret == 0);
    assert(dlen == 5);
    assert(memcmp(decoded, "Hello", 5) == 0);

    // Base64 roundtrip with longer data
    const char* data = "ESP32 mbedtls base64 test!";
    size_t data_len = strlen(data);
    unsigned char enc2[128], dec2[128];
    mbedtls_base64_encode(enc2, sizeof(enc2), &olen, (const unsigned char*)data, data_len);
    mbedtls_base64_decode(dec2, sizeof(dec2), &dlen, enc2, olen);
    assert(dlen == data_len);
    assert(memcmp(dec2, data, data_len) == 0);

    // Buffer too small
    unsigned char tiny[4];
    ret = mbedtls_base64_encode(tiny, sizeof(tiny), &olen, (const unsigned char*)"Hello", 5);
    assert(ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL);

    printf("test_mbedtls: all assertions passed\n");
    return 0;
}
