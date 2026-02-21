// esp32emu test — mbedtls entropy + CTR-DRBG mock
#include <cassert>
#include <cstring>
#include <cstdio>
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

int main() {
    // Test entropy init/free
    {
        mbedtls_entropy_context entropy;
        mbedtls_entropy_init(&entropy);
        assert(entropy.initialized == 1);
        mbedtls_entropy_free(&entropy);
        assert(entropy.initialized == 0);
    }

    // Test entropy_func produces output
    {
        unsigned char buf[32] = {};
        assert(mbedtls_entropy_func(nullptr, buf, 32) == 0);
        // Should have written something (extremely unlikely all zeros)
        int nonzero = 0;
        for (int i = 0; i < 32; i++) if (buf[i]) nonzero++;
        assert(nonzero > 0);
    }

    // Test CTR-DRBG init/seed/free
    {
        mbedtls_ctr_drbg_context ctr;
        mbedtls_entropy_context entropy;
        mbedtls_ctr_drbg_init(&ctr);
        mbedtls_entropy_init(&entropy);

        const char* pers = "esp32emu_test";
        assert(mbedtls_ctr_drbg_seed(&ctr, mbedtls_entropy_func, &entropy,
               (const unsigned char*)pers, strlen(pers)) == 0);
        assert(ctr.initialized == 1);

        mbedtls_ctr_drbg_free(&ctr);
        mbedtls_entropy_free(&entropy);
    }

    // Test CTR-DRBG random output
    {
        mbedtls_ctr_drbg_context ctr;
        mbedtls_entropy_context entropy;
        mbedtls_ctr_drbg_init(&ctr);
        mbedtls_entropy_init(&entropy);
        mbedtls_ctr_drbg_seed(&ctr, mbedtls_entropy_func, &entropy, nullptr, 0);

        unsigned char buf[64] = {};
        assert(mbedtls_ctr_drbg_random(&ctr, buf, 64) == 0);
        int nonzero = 0;
        for (int i = 0; i < 64; i++) if (buf[i]) nonzero++;
        assert(nonzero > 0);

        mbedtls_ctr_drbg_free(&ctr);
        mbedtls_entropy_free(&entropy);
    }

    // Test set_prediction_resistance, set_entropy_len, reseed (no-ops)
    {
        mbedtls_ctr_drbg_context ctr;
        mbedtls_ctr_drbg_init(&ctr);
        mbedtls_ctr_drbg_set_prediction_resistance(&ctr, 1);
        mbedtls_ctr_drbg_set_entropy_len(&ctr, 48);
        assert(mbedtls_ctr_drbg_reseed(&ctr, nullptr, 0) == 0);
        mbedtls_ctr_drbg_free(&ctr);
    }

    // Test add_source
    {
        mbedtls_entropy_context entropy;
        mbedtls_entropy_init(&entropy);
        assert(mbedtls_entropy_add_source(&entropy, nullptr, nullptr, 32, 1) == 0);
        mbedtls_entropy_free(&entropy);
    }

    printf("test_mbedtls_rng: all assertions passed\n");
    return 0;
}
