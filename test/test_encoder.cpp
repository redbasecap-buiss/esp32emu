// Test: Encoder library mock
#include "Encoder.h"
#include <cassert>
#include <cstdio>

int main() {
    // Basic creation
    Encoder enc(2, 3);
    assert(enc.pinA() == 2);
    assert(enc.pinB() == 3);
    assert(enc.read() == 0);

    // Write/read
    enc.write(100);
    assert(enc.read() == 100);

    // Rotate helper
    enc._rotate(5);
    assert(enc.read() == 105);
    enc._rotate(-10);
    assert(enc.read() == 95);

    // Set position
    enc._setPosition(-50);
    assert(enc.read() == -50);

    // Reset
    enc.write(0);
    assert(enc.read() == 0);

    printf("test_encoder: all assertions passed\n");
    return 0;
}
