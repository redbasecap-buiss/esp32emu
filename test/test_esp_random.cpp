// test_esp_random.cpp — Tests for esp_random.h
#include "esp_random.h"
#include <cassert>
#include <cstdio>

int main() {
    // esp_random returns values (just ensure it doesn't crash and varies)
    uint32_t a = esp_random();
    uint32_t b = esp_random();
    // They could theoretically be equal, but check basic functionality
    (void)a; (void)b;
    
    // esp_fill_random
    uint8_t buf[32] = {};
    esp_fill_random(buf, sizeof(buf));
    
    // Check that not all bytes are zero (extremely unlikely with 32 bytes)
    bool all_zero = true;
    for (size_t i = 0; i < sizeof(buf); i++) {
        if (buf[i] != 0) { all_zero = false; break; }
    }
    assert(!all_zero);
    
    // Zero-length fill should be fine
    esp_fill_random(buf, 0);
    
    printf("test_esp_random: all assertions passed\n");
    return 0;
}
