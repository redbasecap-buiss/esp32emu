#pragma once
// esp32emu — mbedtls CTR-DRBG random number generator mock

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>

typedef struct {
    int initialized;
    unsigned int seed;
} mbedtls_ctr_drbg_context;

inline void mbedtls_ctr_drbg_init(mbedtls_ctr_drbg_context* ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

inline void mbedtls_ctr_drbg_free(mbedtls_ctr_drbg_context* ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

inline int mbedtls_ctr_drbg_seed(mbedtls_ctr_drbg_context* ctx,
                                  int (*f_entropy)(void*, unsigned char*, size_t),
                                  void* p_entropy,
                                  const unsigned char* custom, size_t len) {
    ctx->initialized = 1;
    // Seed from custom data if provided
    if (custom && len > 0) {
        ctx->seed = 0;
        for (size_t i = 0; i < len; i++)
            ctx->seed = ctx->seed * 31 + custom[i];
    } else {
        unsigned char buf[4];
        if (f_entropy) f_entropy(p_entropy, buf, 4);
        ctx->seed = (unsigned int)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
    }
    srand(ctx->seed);
    return 0;
}

inline int mbedtls_ctr_drbg_random(void* p_rng, unsigned char* output, size_t output_len) {
    (void)p_rng;
    for (size_t i = 0; i < output_len; i++) {
        output[i] = (unsigned char)(rand() & 0xFF);
    }
    return 0;
}

inline void mbedtls_ctr_drbg_set_prediction_resistance(mbedtls_ctr_drbg_context* /*ctx*/, int /*resistance*/) {
    // no-op in mock
}

inline void mbedtls_ctr_drbg_set_entropy_len(mbedtls_ctr_drbg_context* /*ctx*/, size_t /*len*/) {
    // no-op in mock
}

inline int mbedtls_ctr_drbg_reseed(mbedtls_ctr_drbg_context* /*ctx*/,
                                    const unsigned char* /*additional*/, size_t /*len*/) {
    return 0;
}
