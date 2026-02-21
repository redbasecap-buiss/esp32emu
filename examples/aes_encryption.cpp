// esp32emu example — AES-128 CBC encryption with mbedTLS
#include <Arduino.h>
#include <mbedtls/aes.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <cstring>

void setup() {
    Serial.begin(115200);
    Serial.println("=== AES-128 CBC Encryption Demo ===");

    // Initialize RNG
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                           (const unsigned char*)"esp32_aes", 9);

    // Generate a random IV
    unsigned char iv[16];
    mbedtls_ctr_drbg_random(&ctr_drbg, iv, 16);

    // Key (in practice, use a secure key store)
    unsigned char key[16] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,
                             0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};

    // Plaintext (padded to 16 bytes)
    unsigned char plaintext[16] = "Hello, ESP32!";
    unsigned char ciphertext[16], decrypted[16];

    // Encrypt
    mbedtls_aes_context aes_enc;
    mbedtls_aes_init(&aes_enc);
    mbedtls_aes_setkey_enc(&aes_enc, key, 128);
    unsigned char iv_copy[16];
    memcpy(iv_copy, iv, 16);
    mbedtls_aes_crypt_cbc(&aes_enc, MBEDTLS_AES_ENCRYPT, 16, iv_copy, plaintext, ciphertext);

    Serial.print("Plaintext:  ");
    Serial.println((char*)plaintext);
    Serial.print("Ciphertext: ");
    for (int i = 0; i < 16; i++) { Serial.printf("%02X", ciphertext[i]); }
    Serial.println();

    // Decrypt
    mbedtls_aes_context aes_dec;
    mbedtls_aes_init(&aes_dec);
    mbedtls_aes_setkey_dec(&aes_dec, key, 128);
    memcpy(iv_copy, iv, 16);
    mbedtls_aes_crypt_cbc(&aes_dec, MBEDTLS_AES_DECRYPT, 16, iv_copy, ciphertext, decrypted);

    Serial.print("Decrypted:  ");
    Serial.println((char*)decrypted);
    Serial.println(memcmp(plaintext, decrypted, 16) == 0 ? "✅ Roundtrip OK!" : "❌ Mismatch!");

    mbedtls_aes_free(&aes_enc);
    mbedtls_aes_free(&aes_dec);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

void loop() { delay(10000); }
