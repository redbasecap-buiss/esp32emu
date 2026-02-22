#include "ESP32Encoder.h"
#include <cassert>
#include <cstdio>

int main() {
    ESP32Encoder encoder;

    // Not attached initially
    assert(!encoder.isAttached());

    // Attach half quad
    encoder.attachHalfQuad(25, 26);
    assert(encoder.isAttached());
    assert(!encoder.isFullQuad());
    assert(encoder.pinA() == 25);
    assert(encoder.pinB() == 26);

    // Count starts at 0
    assert(encoder.getCount() == 0);

    // Simulate ticks
    encoder._simulateTicks(100);
    assert(encoder.getCount() == 100);

    encoder._simulateTicks(-30);
    assert(encoder.getCount() == 70);

    // Clear count
    int64_t old = encoder.clearCount();
    assert(old == 70);
    assert(encoder.getCount() == 0);

    // Set count
    encoder.setCount(500);
    assert(encoder.getCount() == 500);

    // Pause/resume
    encoder.pauseCount();
    encoder._simulateTicks(50);
    assert(encoder.getCount() == 500);  // Paused, no change

    encoder.resumeCount();
    encoder._simulateTicks(50);
    assert(encoder.getCount() == 550);

    // Full quad
    ESP32Encoder enc2;
    enc2.attachFullQuad(10, 11);
    assert(enc2.isFullQuad());

    // Pull resistors (static)
    ESP32Encoder::useInternalWeakPullResistors(ESP32Encoder::UP);

    printf("test_esp32encoder: all assertions passed\n");
    return 0;
}
