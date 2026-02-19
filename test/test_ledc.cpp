// esp32emu — test_ledc: LEDC PWM controller test
#include "Arduino.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    ledc_test::reset();

    // Test setup
    double freq = ledcSetup(0, 5000, 8);
    assert(freq == 5000.0);
    assert(ledc_test::isConfigured(0));
    assert(ledc_test::getFrequency(0) == 5000.0);
    assert(!ledc_test::isConfigured(1)); // channel 1 not configured

    // Test attach pin
    ledcAttachPin(2, 0);
    assert(ledc_test::getPin(0) == 2);

    // Test write/read
    ledcWrite(0, 128);
    assert(ledcRead(0) == 128);
    assert(ledc_test::getDuty(0) == 128);

    // Test readFreq
    assert(ledcReadFreq(0) == 5000.0);

    // Test detach
    ledcDetachPin(2);
    assert(ledc_test::getPin(0) == -1);

    // Test writeTone
    ledcSetup(1, 1000, 10);
    ledcWriteTone(1, 440);
    assert(ledc_test::getFrequency(1) == 440.0);
    assert(ledc_test::getDuty(1) > 0); // 50% duty

    // Test writeTone with 0 freq stops
    ledcWriteTone(1, 0);
    assert(ledc_test::getDuty(1) == 0);

    // Test writeNote (A4 = 440Hz, note=9, octave=4)
    ledcSetup(2, 1000, 10);
    ledcWriteNote(2, 9, 4);
    double noteFreq = ledc_test::getFrequency(2);
    assert(std::abs(noteFreq - 440.0) < 1.0);

    // Test multiple channels
    ledc_test::reset();
    for (uint8_t i = 0; i < 4; i++) {
        ledcSetup(i, 1000 * (i + 1), 8);
        ledcWrite(i, 50 * (i + 1));
    }
    assert(ledc_test::getFrequency(0) == 1000.0);
    assert(ledc_test::getFrequency(3) == 4000.0);
    assert(ledc_test::getDuty(0) == 50);
    assert(ledc_test::getDuty(3) == 200);

    // Test out-of-range channel
    ledcSetup(20, 1000, 8); // should silently ignore
    assert(!ledc_test::isConfigured(15)); // last valid channel

    printf("test_ledc: all assertions passed\n");
    return 0;
}
