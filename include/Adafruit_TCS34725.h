// Adafruit_TCS34725.h — Color sensor mock for esp32emu
#pragma once
#include <cstdint>
#include <cstdio>

typedef enum {
    TCS34725_INTEGRATIONTIME_2_4MS  = 0xFF,
    TCS34725_INTEGRATIONTIME_24MS   = 0xF6,
    TCS34725_INTEGRATIONTIME_50MS   = 0xEB,
    TCS34725_INTEGRATIONTIME_101MS  = 0xD5,
    TCS34725_INTEGRATIONTIME_154MS  = 0xC0,
    TCS34725_INTEGRATIONTIME_700MS  = 0x00
} tcs34725IntegrationTime_t;

typedef enum {
    TCS34725_GAIN_1X  = 0x00,
    TCS34725_GAIN_4X  = 0x01,
    TCS34725_GAIN_16X = 0x02,
    TCS34725_GAIN_60X = 0x03
} tcs34725Gain_t;

class Adafruit_TCS34725 {
public:
    Adafruit_TCS34725(tcs34725IntegrationTime_t it = TCS34725_INTEGRATIONTIME_2_4MS,
                      tcs34725Gain_t gain = TCS34725_GAIN_1X)
        : _it(it), _gain(gain) {}

    bool begin(uint8_t addr = 0x29) {
        (void)addr;
        _begun = true;
        printf("[esp32emu] TCS34725: color sensor initialized\n");
        return true;
    }

    void setIntegrationTime(tcs34725IntegrationTime_t it) { _it = it; }
    tcs34725IntegrationTime_t getIntegrationTime() const { return _it; }

    void setGain(tcs34725Gain_t gain) { _gain = gain; }
    tcs34725Gain_t getGain() const { return _gain; }

    void getRawData(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {
        if (r) *r = _r;
        if (g) *g = _g;
        if (b) *b = _b;
        if (c) *c = _c;
    }

    bool getRawDataOneShot(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {
        getRawData(r, g, b, c);
        return true;
    }

    uint16_t calculateColorTemperature(uint16_t r, uint16_t g, uint16_t b) {
        (void)r; (void)b;
        // Simplified McCamy's formula approximation
        float n = ((float)r - (float)b) / (float)g;
        float cct = 449.0f * n * n * n + 3525.0f * n * n + 6823.3f * n + 5520.33f;
        return (uint16_t)cct;
    }

    uint16_t calculateColorTemperature_dn40(uint16_t r, uint16_t g, uint16_t b, uint16_t c) {
        (void)c;
        return calculateColorTemperature(r, g, b);
    }

    uint16_t calculateLux(uint16_t r, uint16_t g, uint16_t b) {
        return (uint16_t)((-0.32466f * r) + (1.57837f * g) + (-0.73191f * b));
    }

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }
    bool isEnabled() const { return _enabled; }

    void setInterrupt(bool flag) { _interrupt = flag; }
    void clearInterrupt() { _interrupt = false; }

    void setIntLimits(uint16_t low, uint16_t high) { _intLow = low; _intHigh = high; }

    // Test helpers
    void setTestColor(uint16_t r, uint16_t g, uint16_t b, uint16_t c) {
        _r = r; _g = g; _b = b; _c = c;
    }

private:
    tcs34725IntegrationTime_t _it;
    tcs34725Gain_t _gain;
    bool _begun = false, _enabled = true, _interrupt = false;
    uint16_t _r = 200, _g = 180, _b = 150, _c = 530;
    uint16_t _intLow = 0, _intHigh = 0xFFFF;
};
