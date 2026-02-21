#pragma once
// esp32emu — mbedtls message digest mock (SHA-256 only, simplified)
// Provides enough API surface for common ESP-IDF HMAC/hash usage.

#include <cstdint>
#include <cstddef>
#include <cstring>

typedef enum {
    MBEDTLS_MD_NONE = 0,
    MBEDTLS_MD_SHA256 = 6,
    MBEDTLS_MD_SHA1 = 4,
} mbedtls_md_type_t;

typedef struct {
    mbedtls_md_type_t type;
} mbedtls_md_info_t;

typedef struct {
    const mbedtls_md_info_t* md_info;
    uint8_t buffer[512];
    size_t buffer_len;
} mbedtls_md_context_t;

// Simple non-cryptographic hash for testing (NOT real SHA-256)
namespace esp32emu_md {

inline void fake_hash(const uint8_t* input, size_t len, uint8_t output[32]) {
    // FNV-1a inspired, fills 32 bytes deterministically
    uint64_t h1 = 0xcbf29ce484222325ULL;
    uint64_t h2 = 0x100000001b3ULL;
    for (size_t i = 0; i < len; i++) {
        h1 ^= input[i]; h1 *= 0x01000193;
        h2 ^= input[i]; h2 *= 0x01000193 + (i & 0xFF);
    }
    for (int i = 0; i < 8; i++) {
        output[i]      = (h1 >> (i * 8)) & 0xFF;
        output[i + 8]  = (h2 >> (i * 8)) & 0xFF;
        output[i + 16] = ((h1 ^ h2) >> (i * 8)) & 0xFF;
        output[i + 24] = ((h1 + h2) >> (i * 8)) & 0xFF;
    }
}

static const mbedtls_md_info_t sha256_info = { MBEDTLS_MD_SHA256 };
static const mbedtls_md_info_t sha1_info   = { MBEDTLS_MD_SHA1 };

} // namespace esp32emu_md

inline const mbedtls_md_info_t* mbedtls_md_info_from_type(mbedtls_md_type_t type) {
    if (type == MBEDTLS_MD_SHA256) return &esp32emu_md::sha256_info;
    if (type == MBEDTLS_MD_SHA1) return &esp32emu_md::sha1_info;
    return nullptr;
}

inline int mbedtls_md_get_size(const mbedtls_md_info_t* info) {
    if (!info) return 0;
    if (info->type == MBEDTLS_MD_SHA256) return 32;
    if (info->type == MBEDTLS_MD_SHA1) return 20;
    return 0;
}

inline void mbedtls_md_init(mbedtls_md_context_t* ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

inline void mbedtls_md_free(mbedtls_md_context_t* ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

inline int mbedtls_md_setup(mbedtls_md_context_t* ctx, const mbedtls_md_info_t* info, int /*hmac*/) {
    ctx->md_info = info;
    ctx->buffer_len = 0;
    return 0;
}

inline int mbedtls_md_starts(mbedtls_md_context_t* ctx) {
    ctx->buffer_len = 0;
    return 0;
}

inline int mbedtls_md_update(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen) {
    size_t copy = (ilen < sizeof(ctx->buffer) - ctx->buffer_len) ? ilen : sizeof(ctx->buffer) - ctx->buffer_len;
    memcpy(ctx->buffer + ctx->buffer_len, input, copy);
    ctx->buffer_len += copy;
    return 0;
}

inline int mbedtls_md_finish(mbedtls_md_context_t* ctx, unsigned char* output) {
    esp32emu_md::fake_hash(ctx->buffer, ctx->buffer_len, output);
    return 0;
}

// One-shot
inline int mbedtls_md(const mbedtls_md_info_t* /*info*/, const unsigned char* input, size_t ilen, unsigned char* output) {
    esp32emu_md::fake_hash(input, ilen, output);
    return 0;
}

// HMAC one-shot
inline int mbedtls_md_hmac(const mbedtls_md_info_t* /*info*/, const unsigned char* key, size_t keylen,
                            const unsigned char* input, size_t ilen, unsigned char* output) {
    // Simplified: hash(key + input)
    uint8_t combined[1024];
    size_t total = (keylen + ilen < sizeof(combined)) ? keylen + ilen : sizeof(combined);
    size_t kl = (keylen < sizeof(combined)) ? keylen : sizeof(combined);
    memcpy(combined, key, kl);
    size_t il = (ilen < sizeof(combined) - kl) ? ilen : sizeof(combined) - kl;
    memcpy(combined + kl, input, il);
    esp32emu_md::fake_hash(combined, total, output);
    return 0;
}

// HMAC streaming
inline int mbedtls_md_hmac_starts(mbedtls_md_context_t* ctx, const unsigned char* key, size_t keylen) {
    ctx->buffer_len = 0;
    size_t kl = (keylen < sizeof(ctx->buffer)) ? keylen : sizeof(ctx->buffer);
    memcpy(ctx->buffer, key, kl);
    ctx->buffer_len = kl;
    return 0;
}

inline int mbedtls_md_hmac_update(mbedtls_md_context_t* ctx, const unsigned char* input, size_t ilen) {
    return mbedtls_md_update(ctx, input, ilen);
}

inline int mbedtls_md_hmac_finish(mbedtls_md_context_t* ctx, unsigned char* output) {
    return mbedtls_md_finish(ctx, output);
}
