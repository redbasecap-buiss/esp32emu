// esp32emu test — mbedtls AES mock
#include <cassert>
#include <cstring>
#include <cstdio>
#include "mbedtls/aes.h"

int main() {
    // Test init/free
    {
        mbedtls_aes_context ctx;
        mbedtls_aes_init(&ctx);
        assert(ctx.key_bits == 0);
        mbedtls_aes_free(&ctx);
    }

    // Test setkey with valid lengths
    {
        mbedtls_aes_context ctx;
        mbedtls_aes_init(&ctx);
        unsigned char key16[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
        assert(mbedtls_aes_setkey_enc(&ctx, key16, 128) == 0);
        assert(ctx.key_bits == 128);
        assert(ctx.mode == MBEDTLS_AES_ENCRYPT);

        unsigned char key24[24] = {};
        assert(mbedtls_aes_setkey_dec(&ctx, key24, 192) == 0);
        assert(ctx.key_bits == 192);
        assert(ctx.mode == MBEDTLS_AES_DECRYPT);

        unsigned char key32[32] = {};
        assert(mbedtls_aes_setkey_enc(&ctx, key32, 256) == 0);
        assert(ctx.key_bits == 256);
        mbedtls_aes_free(&ctx);
    }

    // Test invalid key length
    {
        mbedtls_aes_context ctx;
        mbedtls_aes_init(&ctx);
        unsigned char key[16] = {};
        assert(mbedtls_aes_setkey_enc(&ctx, key, 64) == MBEDTLS_ERR_AES_INVALID_KEY_LENGTH);
        mbedtls_aes_free(&ctx);
    }

    // Test ECB encrypt/decrypt roundtrip
    {
        mbedtls_aes_context enc, dec;
        mbedtls_aes_init(&enc);
        mbedtls_aes_init(&dec);
        unsigned char key[16] = {0xAA,0xBB,0xCC,0xDD,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0x00,0xEE,0xFF};
        mbedtls_aes_setkey_enc(&enc, key, 128);
        mbedtls_aes_setkey_dec(&dec, key, 128);

        unsigned char plain[16] = "Hello, AES!";
        unsigned char cipher[16], recovered[16];
        assert(mbedtls_aes_crypt_ecb(&enc, MBEDTLS_AES_ENCRYPT, plain, cipher) == 0);
        // cipher should differ from plain
        assert(memcmp(plain, cipher, 16) != 0);
        // Decrypt back
        assert(mbedtls_aes_crypt_ecb(&dec, MBEDTLS_AES_DECRYPT, cipher, recovered) == 0);
        assert(memcmp(plain, recovered, 16) == 0);

        mbedtls_aes_free(&enc);
        mbedtls_aes_free(&dec);
    }

    // Test CBC encrypt/decrypt roundtrip
    {
        mbedtls_aes_context enc, dec;
        mbedtls_aes_init(&enc);
        mbedtls_aes_init(&dec);
        unsigned char key[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
        mbedtls_aes_setkey_enc(&enc, key, 128);
        mbedtls_aes_setkey_dec(&dec, key, 128);

        unsigned char iv_enc[16] = {0}, iv_dec[16] = {0};
        unsigned char plain[32] = "AES CBC mode test!!12345678";
        unsigned char cipher[32], recovered[32];

        assert(mbedtls_aes_crypt_cbc(&enc, MBEDTLS_AES_ENCRYPT, 32, iv_enc, plain, cipher) == 0);
        assert(memcmp(plain, cipher, 32) != 0);
        assert(mbedtls_aes_crypt_cbc(&dec, MBEDTLS_AES_DECRYPT, 32, iv_dec, cipher, recovered) == 0);
        assert(memcmp(plain, recovered, 32) == 0);

        mbedtls_aes_free(&enc);
        mbedtls_aes_free(&dec);
    }

    // Test CBC invalid length
    {
        mbedtls_aes_context ctx;
        mbedtls_aes_init(&ctx);
        unsigned char key[16] = {};
        mbedtls_aes_setkey_enc(&ctx, key, 128);
        unsigned char iv[16] = {}, buf[17] = {}, out[17] = {};
        assert(mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, 17, iv, buf, out) == MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH);
        mbedtls_aes_free(&ctx);
    }

    printf("test_mbedtls_aes: all assertions passed\n");
    return 0;
}
