#pragma once
// esp32emu — mbedtls AES mock (128/192/256-bit, ECB/CBC)

#include <cstdint>
#include <cstring>
#include <cstddef>

#define MBEDTLS_AES_ENCRYPT 1
#define MBEDTLS_AES_DECRYPT 0
#define MBEDTLS_ERR_AES_INVALID_KEY_LENGTH -0x0020
#define MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH -0x0022

typedef struct {
    uint8_t key[32];
    unsigned int key_bits;
    int mode; // 0=decrypt, 1=encrypt
} mbedtls_aes_context;

inline void mbedtls_aes_init(mbedtls_aes_context* ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

inline void mbedtls_aes_free(mbedtls_aes_context* ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

inline int mbedtls_aes_setkey_enc(mbedtls_aes_context* ctx, const unsigned char* key, unsigned int keybits) {
    if (keybits != 128 && keybits != 192 && keybits != 256)
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    ctx->key_bits = keybits;
    ctx->mode = MBEDTLS_AES_ENCRYPT;
    memcpy(ctx->key, key, keybits / 8);
    return 0;
}

inline int mbedtls_aes_setkey_dec(mbedtls_aes_context* ctx, const unsigned char* key, unsigned int keybits) {
    if (keybits != 128 && keybits != 192 && keybits != 256)
        return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    ctx->key_bits = keybits;
    ctx->mode = MBEDTLS_AES_DECRYPT;
    memcpy(ctx->key, key, keybits / 8);
    return 0;
}

// Simple XOR-based mock (NOT real AES — for testing API usage)
inline int mbedtls_aes_crypt_ecb(mbedtls_aes_context* ctx, int /*mode*/,
                                  const unsigned char input[16], unsigned char output[16]) {
    for (int i = 0; i < 16; i++) {
        output[i] = input[i] ^ ctx->key[i % (ctx->key_bits / 8)];
    }
    return 0;
}

inline int mbedtls_aes_crypt_cbc(mbedtls_aes_context* ctx, int mode, size_t length,
                                  unsigned char iv[16],
                                  const unsigned char* input, unsigned char* output) {
    if (length % 16 != 0)
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    unsigned char tmp[16];
    for (size_t offset = 0; offset < length; offset += 16) {
        if (mode == MBEDTLS_AES_ENCRYPT) {
            for (int i = 0; i < 16; i++)
                tmp[i] = input[offset + i] ^ iv[i];
            mbedtls_aes_crypt_ecb(ctx, mode, tmp, output + offset);
            memcpy(iv, output + offset, 16);
        } else {
            mbedtls_aes_crypt_ecb(ctx, mode, input + offset, tmp);
            for (int i = 0; i < 16; i++)
                output[offset + i] = tmp[i] ^ iv[i];
            memcpy(iv, input + offset, 16);
        }
    }
    return 0;
}
