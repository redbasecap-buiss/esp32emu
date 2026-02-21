#pragma once
#include "Arduino.h"
#include "Wire.h"

// esp32emu: VEML7700 Ambient Light Sensor mock
class Adafruit_VEML7700 {
public:
    Adafruit_VEML7700() = default;

    bool begin(TwoWire *wire = &Wire) {
        (void)wire;
        _initialized = true;
        Serial.println("[esp32emu] VEML7700: initialized");
        return true;
    }

    // Gain settings
    enum { VEML7700_GAIN_1 = 0, VEML7700_GAIN_2 = 1, VEML7700_GAIN_1_8 = 2, VEML7700_GAIN_1_4 = 3 };
    // Integration time
    enum { VEML7700_IT_25MS = 0x0C, VEML7700_IT_50MS = 0x08, VEML7700_IT_100MS = 0x00,
           VEML7700_IT_200MS = 0x01, VEML7700_IT_400MS = 0x02, VEML7700_IT_800MS = 0x03 };

    void setGain(uint8_t gain) { _gain = gain; }
    uint8_t getGain() const { return _gain; }
    void setIntegrationTime(uint8_t it) { _it = it; }
    uint8_t getIntegrationTime() const { return _it; }

    float readLux() const { return _lux; }
    float readLuxNormalized() const { return _lux; }
    uint16_t readALS() const { return _als; }
    uint16_t readWhite() const { return _white; }
    float readWhiteNormalized() const { return (float)_white; }

    void interruptEnable(bool enable) { _intEnable = enable; }
    void setLowThreshold(uint16_t low) { _lowThresh = low; }
    void setHighThreshold(uint16_t high) { _highThresh = high; }
    uint16_t interruptStatus() const { return 0; }

    void powerSaveEnable(bool enable) { _powerSave = enable; }
    void setPowerSaveMode(uint8_t mode) { _psMode = mode; }

    // Test helpers
    void emu_setLux(float lux) { _lux = lux; }
    void emu_setALS(uint16_t als) { _als = als; }
    void emu_setWhite(uint16_t white) { _white = white; }
    bool emu_initialized() const { return _initialized; }

private:
    bool _initialized = false;
    float _lux = 250.0f;
    uint16_t _als = 1000;
    uint16_t _white = 1100;
    uint8_t _gain = 0;
    uint8_t _it = 0;
    bool _intEnable = false;
    uint16_t _lowThresh = 0;
    uint16_t _highThresh = 0xFFFF;
    bool _powerSave = false;
    uint8_t _psMode = 0;
};
