#pragma once
// esp32emu — mbedtls SHA-256 mock

#include "md.h"

typedef struct {
    uint8_t buffer[512];
    size_t buffer_len;
    int is224;
} mbedtls_sha256_context;

inline void mbedtls_sha256_init(mbedtls_sha256_context* ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

inline void mbedtls_sha256_free(mbedtls_sha256_context* ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

inline int mbedtls_sha256_starts(mbedtls_sha256_context* ctx, int is224) {
    ctx->is224 = is224;
    ctx->buffer_len = 0;
    return 0;
}

// Compat alias
inline int mbedtls_sha256_starts_ret(mbedtls_sha256_context* ctx, int is224) {
    return mbedtls_sha256_starts(ctx, is224);
}

inline int mbedtls_sha256_update(mbedtls_sha256_context* ctx, const unsigned char* input, size_t ilen) {
    size_t copy = (ilen < sizeof(ctx->buffer) - ctx->buffer_len) ? ilen : sizeof(ctx->buffer) - ctx->buffer_len;
    memcpy(ctx->buffer + ctx->buffer_len, input, copy);
    ctx->buffer_len += copy;
    return 0;
}

inline int mbedtls_sha256_update_ret(mbedtls_sha256_context* ctx, const unsigned char* input, size_t ilen) {
    return mbedtls_sha256_update(ctx, input, ilen);
}

inline int mbedtls_sha256_finish(mbedtls_sha256_context* ctx, unsigned char output[32]) {
    esp32emu_md::fake_hash(ctx->buffer, ctx->buffer_len, output);
    return 0;
}

inline int mbedtls_sha256_finish_ret(mbedtls_sha256_context* ctx, unsigned char output[32]) {
    return mbedtls_sha256_finish(ctx, output);
}

// One-shot
inline int mbedtls_sha256(const unsigned char* input, size_t ilen, unsigned char output[32], int /*is224*/) {
    esp32emu_md::fake_hash(input, ilen, output);
    return 0;
}

// Compat
inline int mbedtls_sha256_ret(const unsigned char* input, size_t ilen, unsigned char output[32], int is224) {
    return mbedtls_sha256(input, ilen, output, is224);
}
