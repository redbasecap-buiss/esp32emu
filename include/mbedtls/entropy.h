#pragma once
// esp32emu — mbedtls entropy source mock

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>

typedef struct {
    int initialized;
} mbedtls_entropy_context;

typedef int (*mbedtls_entropy_f_source_t)(void* data, unsigned char* output, size_t len, size_t* olen);

inline void mbedtls_entropy_init(mbedtls_entropy_context* ctx) {
    ctx->initialized = 1;
}

inline void mbedtls_entropy_free(mbedtls_entropy_context* ctx) {
    ctx->initialized = 0;
}

inline int mbedtls_entropy_func(void* /*data*/, unsigned char* output, size_t len) {
    // Mock: fill with pseudo-random bytes
    for (size_t i = 0; i < len; i++) {
        output[i] = (unsigned char)(rand() & 0xFF);
    }
    return 0;
}

inline int mbedtls_entropy_add_source(mbedtls_entropy_context* /*ctx*/,
                                       mbedtls_entropy_f_source_t /*f_source*/,
                                       void* /*p_source*/, size_t /*threshold*/,
                                       int /*strong*/) {
    return 0;
}
