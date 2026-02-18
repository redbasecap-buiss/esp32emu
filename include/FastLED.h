#pragma once
// esp32emu: FastLED mock — popular WS2812/APA102 LED library
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>

struct CRGB {
    uint8_t r, g, b;
    CRGB() : r(0), g(0), b(0) {}
    CRGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    CRGB(uint32_t colorcode) : r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b(colorcode & 0xFF) {}
    bool operator==(const CRGB& o) const { return r == o.r && g == o.g && b == o.b; }
    bool operator!=(const CRGB& o) const { return !(*this == o); }
    CRGB& operator|=(const CRGB& o) { r = std::max(r, o.r); g = std::max(g, o.g); b = std::max(b, o.b); return *this; }
    CRGB& nscale8(uint8_t scale) { r = (uint16_t(r) * scale) >> 8; g = (uint16_t(g) * scale) >> 8; b = (uint16_t(b) * scale) >> 8; return *this; }
    CRGB& fadeToBlackBy(uint8_t amount) { return nscale8(255 - amount); }

    // Named colors
    static const uint32_t Red     = 0xFF0000;
    static const uint32_t Green   = 0x00FF00;
    static const uint32_t Blue    = 0x0000FF;
    static const uint32_t White   = 0xFFFFFF;
    static const uint32_t Black   = 0x000000;
    static const uint32_t Yellow  = 0xFFFF00;
    static const uint32_t Cyan    = 0x00FFFF;
    static const uint32_t Magenta = 0xFF00FF;
    static const uint32_t Orange  = 0xFF8000;
};

struct CHSV {
    uint8_t h, s, v;
    CHSV() : h(0), s(0), v(0) {}
    CHSV(uint8_t h, uint8_t s, uint8_t v) : h(h), s(s), v(v) {}
};

inline CRGB hsv2rgb_rainbow(const CHSV& hsv) {
    // Simplified HSV→RGB for mock
    uint8_t region = hsv.h / 43;
    uint8_t remainder = (hsv.h - region * 43) * 6;
    uint8_t p = (hsv.v * (255 - hsv.s)) >> 8;
    uint8_t q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    uint8_t t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;
    switch (region) {
        case 0: return CRGB(hsv.v, t, p);
        case 1: return CRGB(q, hsv.v, p);
        case 2: return CRGB(p, hsv.v, t);
        case 3: return CRGB(p, q, hsv.v);
        case 4: return CRGB(t, p, hsv.v);
        default: return CRGB(hsv.v, p, q);
    }
}

inline void hsv2rgb_rainbow(const CHSV& hsv, CRGB& rgb) { rgb = hsv2rgb_rainbow(hsv); }

enum EOrder { RGB = 0x012, GRB = 0x102, BRG = 0x201 };

// Chipset types (unused but needed for template)
enum ESPIChipsets { APA102 };
enum EClocklessChipsets { WS2812, WS2812B, WS2811, NEOPIXEL, WS2813, SK6812 };

inline uint8_t beatsin8(uint8_t bpm, uint8_t low = 0, uint8_t high = 255, uint32_t timebase = 0, uint8_t phase = 0) {
    (void)timebase; (void)phase; (void)bpm;
    return (low + high) / 2; // simplified
}

inline void fill_rainbow(CRGB* leds, int num, uint8_t hue, uint8_t deltaHue = 5) {
    for (int i = 0; i < num; i++) {
        leds[i] = hsv2rgb_rainbow(CHSV(hue, 240, 255));
        hue += deltaHue;
    }
}

inline void fill_solid(CRGB* leds, int num, const CRGB& color) {
    for (int i = 0; i < num; i++) leds[i] = color;
}

class CFastLED {
public:
    static CFastLED& instance() { static CFastLED inst; return inst; }

    // 4-param: chipset, data, clock, order (SPI LEDs like APA102)
    template<int CHIPSET, int DATA_PIN, int CLOCK_PIN, int ORDER>
    CFastLED& addLeds(CRGB* leds, int num) { leds_ = leds; num_ = num; return *this; }

    // 3-param: chipset, data, order (most common: WS2812B)
    template<int CHIPSET, int DATA_PIN, int ORDER>
    CFastLED& addLeds(CRGB* leds, int num) { leds_ = leds; num_ = num; return *this; }

    CFastLED& setCorrection(uint32_t) { return *this; }
    CFastLED& setTemperature(uint32_t) { return *this; }

    void setBrightness(uint8_t b) { brightness_ = b; }
    uint8_t getBrightness() const { return brightness_; }

    void show() { showCount_++; }
    void clear(bool writeData = false) {
        if (leds_ && writeData) fill_solid(leds_, num_, CRGB(0, 0, 0));
        if (leds_) fill_solid(leds_, num_, CRGB(0, 0, 0));
    }

    void setMaxPowerInVoltsAndMilliamps(uint8_t, uint32_t) {}
    void delay(unsigned long ms) { show(); (void)ms; }

    // Test helpers
    int getShowCount() const { return showCount_; }
    CRGB* getLeds() const { return leds_; }
    int getNumLeds() const { return num_; }
    void resetShowCount() { showCount_ = 0; }

private:
    CFastLED() : leds_(nullptr), num_(0), brightness_(255), showCount_(0) {}
    CRGB* leds_;
    int num_;
    uint8_t brightness_;
    int showCount_;
};

static CFastLED& FastLED = CFastLED::instance();
