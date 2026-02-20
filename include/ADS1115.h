#pragma once
#include <cstdint>
#include <cstdio>
#include <cmath>
#include "Wire.h"

// ADS1115 — 16-bit 4-channel I2C ADC mock
// Texas Instruments ADS1115 — very popular for precision analog readings

#define ADS1115_ADDRESS_GND  0x48
#define ADS1115_ADDRESS_VDD  0x49
#define ADS1115_ADDRESS_SDA  0x4A
#define ADS1115_ADDRESS_SCL  0x4B
#define ADS1115_ADDRESS      ADS1115_ADDRESS_GND

// Gain settings (full-scale range)
enum adsGain_t {
    GAIN_TWOTHIRDS = 0x0000,  // +/- 6.144V  (187.5µV/bit)
    GAIN_ONE       = 0x0200,  // +/- 4.096V  (125µV/bit)
    GAIN_TWO       = 0x0400,  // +/- 2.048V  (62.5µV/bit, default)
    GAIN_FOUR      = 0x0600,  // +/- 1.024V  (31.25µV/bit)
    GAIN_EIGHT     = 0x0800,  // +/- 0.512V  (15.625µV/bit)
    GAIN_SIXTEEN   = 0x0A00,  // +/- 0.256V  (7.8125µV/bit)
};

// Data rate settings
enum adsDataRate_t {
    DR_8SPS   = 0x0000,
    DR_16SPS  = 0x0020,
    DR_32SPS  = 0x0040,
    DR_64SPS  = 0x0060,
    DR_128SPS = 0x0080,  // default
    DR_250SPS = 0x00A0,
    DR_475SPS = 0x00C0,
    DR_860SPS = 0x00E0,
};

class ADS1115 {
public:
    ADS1115(uint8_t addr = ADS1115_ADDRESS) : _addr(addr) {}

    bool begin(TwoWire* wire = nullptr) {
        _initialized = true;
        printf("[ADS1115] Initialized at 0x%02X\n", _addr);
        return true;
    }

    void setGain(adsGain_t gain) { _gain = gain; }
    adsGain_t getGain() { return _gain; }

    void setDataRate(adsDataRate_t rate) { _dataRate = rate; }
    adsDataRate_t getDataRate() { return _dataRate; }

    // Read single-ended (0-3)
    int16_t readADC_SingleEnded(uint8_t channel) {
        if (channel > 3) return 0;
        return _channelValues[channel];
    }

    // Read differential: AIN0 - AIN1
    int16_t readADC_Differential_0_1() {
        return _channelValues[0] - _channelValues[1];
    }

    // Read differential: AIN2 - AIN3
    int16_t readADC_Differential_2_3() {
        return _channelValues[2] - _channelValues[3];
    }

    // Convert raw ADC value to voltage
    float computeVolts(int16_t raw) {
        float fsRange;
        switch (_gain) {
            case GAIN_TWOTHIRDS: fsRange = 6.144f; break;
            case GAIN_ONE:       fsRange = 4.096f; break;
            case GAIN_TWO:       fsRange = 2.048f; break;
            case GAIN_FOUR:      fsRange = 1.024f; break;
            case GAIN_EIGHT:     fsRange = 0.512f; break;
            case GAIN_SIXTEEN:   fsRange = 0.256f; break;
            default:             fsRange = 2.048f; break;
        }
        return (float)raw * fsRange / 32767.0f;
    }

    // Comparator mode
    void startComparator_SingleEnded(uint8_t channel, int16_t threshold) {
        _compChannel = channel;
        _compThreshold = threshold;
        _compMode = true;
    }

    int16_t getLastConversionResults() {
        if (_compChannel > 3) return 0;
        return _channelValues[_compChannel];
    }

    // Test helpers
    void test_setChannelValue(uint8_t channel, int16_t value) {
        if (channel > 3) return;
        _channelValues[channel] = value;
    }

    void test_setAllChannels(int16_t ch0, int16_t ch1, int16_t ch2, int16_t ch3) {
        _channelValues[0] = ch0;
        _channelValues[1] = ch1;
        _channelValues[2] = ch2;
        _channelValues[3] = ch3;
    }

    bool isInitialized() { return _initialized; }

    void reset() {
        _initialized = false;
        _gain = GAIN_TWO;
        _dataRate = DR_128SPS;
        _compMode = false;
        _compChannel = 0;
        _compThreshold = 0;
        for (int i = 0; i < 4; i++) _channelValues[i] = 0;
    }

private:
    uint8_t      _addr;
    bool         _initialized = false;
    adsGain_t    _gain = GAIN_TWO;
    adsDataRate_t _dataRate = DR_128SPS;
    int16_t      _channelValues[4] = {0, 0, 0, 0};
    bool         _compMode = false;
    uint8_t      _compChannel = 0;
    int16_t      _compThreshold = 0;
};
