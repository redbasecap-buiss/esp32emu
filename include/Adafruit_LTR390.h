#pragma once
#include "Arduino.h"
#include "Wire.h"

// LTR390 UV / Ambient Light Sensor mock
// Modes
enum ltr390_mode_t {
    LTR390_MODE_ALS = 0,
    LTR390_MODE_UVS = 1
};

enum ltr390_gain_t {
    LTR390_GAIN_1 = 0,
    LTR390_GAIN_3 = 1,
    LTR390_GAIN_6 = 2,
    LTR390_GAIN_9 = 3,
    LTR390_GAIN_18 = 4
};

enum ltr390_resolution_t {
    LTR390_RESOLUTION_20BIT = 0,
    LTR390_RESOLUTION_19BIT = 1,
    LTR390_RESOLUTION_18BIT = 2,
    LTR390_RESOLUTION_17BIT = 3,
    LTR390_RESOLUTION_16BIT = 4,
    LTR390_RESOLUTION_13BIT = 5
};

class Adafruit_LTR390 {
public:
    Adafruit_LTR390() {}

    bool begin(TwoWire* wire = &Wire) {
        (void)wire;
        _initialized = true;
        return true;
    }

    void enable(bool en) { _enabled = en; }
    bool enabled() const { return _enabled; }

    void setMode(ltr390_mode_t mode) { _mode = mode; }
    ltr390_mode_t getMode() const { return _mode; }

    void setGain(ltr390_gain_t gain) { _gain = gain; }
    ltr390_gain_t getGain() const { return _gain; }

    void setResolution(ltr390_resolution_t res) { _resolution = res; }
    ltr390_resolution_t getResolution() const { return _resolution; }

    void setThresholds(uint32_t lower, uint32_t upper) {
        _threshLower = lower;
        _threshUpper = upper;
    }

    void configInterrupt(bool enable, ltr390_mode_t src, uint8_t persistance = 0) {
        _intEnable = enable;
        _intSrc = src;
        _intPersist = persistance;
    }

    bool newDataAvailable() const { return _initialized && _enabled; }

    uint32_t readALS() const { return _alsData; }
    uint32_t readUVS() const { return _uvsData; }

    // Calculate lux from ALS data (simplified)
    float getLux() const {
        float gain_factor[] = {1, 3, 6, 9, 18};
        float res_factor[] = {4.0f, 2.0f, 1.0f, 0.5f, 0.25f, 0.03125f};
        float g = gain_factor[_gain];
        float r = res_factor[_resolution];
        return 0.6f * (float)_alsData / (g * r);
    }

    // Calculate UV index from UVS data (simplified)
    float getUVI() const {
        float gain_factor[] = {1, 3, 6, 9, 18};
        float res_factor[] = {4.0f, 2.0f, 1.0f, 0.5f, 0.25f, 0.03125f};
        float g = gain_factor[_gain];
        float r = res_factor[_resolution];
        return (float)_uvsData / (g * r) * 2.0f;
    }

    // ── Test helpers ──
    void _setALS(uint32_t val) { _alsData = val; }
    void _setUVS(uint32_t val) { _uvsData = val; }

private:
    bool _initialized = false;
    bool _enabled = false;
    ltr390_mode_t _mode = LTR390_MODE_ALS;
    ltr390_gain_t _gain = LTR390_GAIN_3;
    ltr390_resolution_t _resolution = LTR390_RESOLUTION_18BIT;
    uint32_t _threshLower = 0;
    uint32_t _threshUpper = 0;
    bool _intEnable = false;
    ltr390_mode_t _intSrc = LTR390_MODE_ALS;
    uint8_t _intPersist = 0;
    uint32_t _alsData = 500;
    uint32_t _uvsData = 120;
};
