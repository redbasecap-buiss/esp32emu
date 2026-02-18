// test_fastled: FastLED library mock tests
#include "FastLED.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test CRGB basics
    CRGB c1(255, 0, 0);
    assert(c1.r == 255 && c1.g == 0 && c1.b == 0);

    CRGB c2(0x00FF00);
    assert(c2.r == 0 && c2.g == 255 && c2.b == 0);

    assert(c1 != c2);
    CRGB c3(255, 0, 0);
    assert(c1 == c3);

    // Test CHSV and conversion
    CHSV hsv(0, 255, 255);
    CRGB rgb = hsv2rgb_rainbow(hsv);
    assert(rgb.r > 200); // red-ish

    // Test FastLED controller
    const int NUM_LEDS = 10;
    CRGB leds[NUM_LEDS];

    FastLED.addLeds<WS2812B, 5, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(128);
    assert(FastLED.getBrightness() == 128);
    assert(FastLED.getNumLeds() == NUM_LEDS);

    // Set colors and show
    leds[0] = CRGB(CRGB::Red);
    leds[1] = CRGB(CRGB::Green);
    leds[2] = CRGB(CRGB::Blue);
    FastLED.resetShowCount();
    FastLED.show();
    assert(FastLED.getShowCount() == 1);
    FastLED.show();
    assert(FastLED.getShowCount() == 2);

    // Test fill_solid
    fill_solid(leds, NUM_LEDS, CRGB(CRGB::White));
    for (int i = 0; i < NUM_LEDS; i++) {
        assert(leds[i] == CRGB(255, 255, 255));
    }

    // Test fill_rainbow
    fill_rainbow(leds, NUM_LEDS, 0, 25);
    // Each LED should be different
    assert(leds[0] != leds[3]);

    // Test fadeToBlackBy
    leds[0] = CRGB(200, 100, 50);
    leds[0].fadeToBlackBy(128);
    assert(leds[0].r < 200);
    assert(leds[0].g < 100);

    // Test clear
    fill_solid(leds, NUM_LEDS, CRGB(CRGB::Red));
    FastLED.clear(true);
    for (int i = 0; i < NUM_LEDS; i++) {
        assert(leds[i] == CRGB(0, 0, 0));
    }

    printf("test_fastled: all assertions passed\n");
    return 0;
}
