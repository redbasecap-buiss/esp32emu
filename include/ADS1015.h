#pragma once
#include "Arduino.h"
#include <cstdio>
#include <cstdint>

// ADS1015 12-bit / ADS1115 16-bit ADC mock (Adafruit style)
// ADS1115.h already exists, this provides the ADS1015 variant

class Adafruit_ADS1015 {
public:
    Adafruit_ADS1015(uint8_t addr = 0x48) : _addr(addr), _begun(false), _gain(1) {
        for (int i = 0; i < 4; i++) _values[i] = 0;
    }

    bool begin(uint8_t addr = 0xFF) {
        if (addr != 0xFF) _addr = addr;
        _begun = true;
        printf("[esp32emu] ADS1015: initialized (addr=0x%02X)\n", _addr);
        return true;
    }

    enum adsGain_t {
        GAIN_TWOTHIRDS = 0,
        GAIN_ONE = 1,
        GAIN_TWO = 2,
        GAIN_FOUR = 4,
        GAIN_EIGHT = 8,
        GAIN_SIXTEEN = 16
    };

    void setGain(adsGain_t gain) { _gain = gain; }
    adsGain_t getGain() const { return (adsGain_t)_gain; }

    int16_t readADC_SingleEnded(uint8_t channel) {
        if (channel > 3) return 0;
        return _values[channel];
    }

    int16_t readADC_Differential_0_1() { return _values[0] - _values[1]; }
    int16_t readADC_Differential_2_3() { return _values[2] - _values[3]; }

    float computeVolts(int16_t counts) {
        float fsRange;
        switch (_gain) {
            case GAIN_TWOTHIRDS: fsRange = 6.144f; break;
            case GAIN_ONE:       fsRange = 4.096f; break;
            case GAIN_TWO:       fsRange = 2.048f; break;
            case GAIN_FOUR:      fsRange = 1.024f; break;
            case GAIN_EIGHT:     fsRange = 0.512f; break;
            case GAIN_SIXTEEN:   fsRange = 0.256f; break;
            default:             fsRange = 6.144f; break;
        }
        return counts * (fsRange / 2048.0f); // 12-bit for ADS1015
    }

    // Test helpers
    void test_setValue(uint8_t channel, int16_t value) {
        if (channel < 4) _values[channel] = value;
    }

private:
    uint8_t _addr;
    bool _begun;
    int _gain;
    int16_t _values[4];
};
