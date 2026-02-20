#pragma once
/*  esp_random.h — ESP-IDF hardware RNG mock for esp32emu */

#include <cstdint>
#include <cstddef>
#include <cstdlib>

#ifdef __cplusplus
extern "C" {
#endif

/// Return a 32-bit random number (uses host PRNG)
inline uint32_t esp_random() {
    return ((uint32_t)rand() << 16) ^ (uint32_t)rand();
}

/// Fill buffer with random bytes
inline void esp_fill_random(void* buf, size_t len) {
    auto* p = static_cast<uint8_t*>(buf);
    for (size_t i = 0; i < len; i++) {
        p[i] = (uint8_t)(rand() & 0xFF);
    }
}

#ifdef __cplusplus
}
#endif
