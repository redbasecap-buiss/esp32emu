#pragma once
#include "Arduino.h"
#include "Wire.h"
#include <cstdio>
#include <cstdint>

// MAX30102/MAX30105 Pulse Oximeter & Heart Rate Sensor mock
class MAX30105 {
public:
    MAX30105() : _begun(false), _irValue(70000), _redValue(65000), _greenValue(0),
                 _sampleRate(100), _ledBrightness(0x1F), _sampleAvg(4),
                 _ledMode(2), _adcRange(4096), _pulseWidth(411),
                 _temperature(25.0f) {}

    bool begin(TwoWire &wire = Wire, uint32_t speed = 400000, uint8_t addr = 0x57) {
        (void)wire; (void)speed; (void)addr;
        _begun = true;
        printf("[esp32emu] MAX30105: initialized (addr=0x%02X)\n", addr);
        return true;
    }

    void setup(uint8_t ledBrightness = 0x1F, uint8_t sampleAvg = 4,
               uint8_t ledMode = 2, int sampleRate = 100,
               int pulseWidth = 411, int adcRange = 4096) {
        _ledBrightness = ledBrightness;
        _sampleAvg = sampleAvg;
        _ledMode = ledMode;
        _sampleRate = sampleRate;
        _pulseWidth = pulseWidth;
        _adcRange = adcRange;
        printf("[esp32emu] MAX30105: setup (LED=0x%02X, avg=%d, mode=%d, rate=%d)\n",
               ledBrightness, sampleAvg, ledMode, sampleRate);
    }

    void setPulseAmplitudeRed(uint8_t val) { _ledBrightness = val; }
    void setPulseAmplitudeIR(uint8_t val) { (void)val; }
    void setPulseAmplitudeGreen(uint8_t val) { (void)val; }

    uint32_t getIR() { return _irValue; }
    uint32_t getRed() { return _redValue; }
    uint32_t getGreen() { return _greenValue; }

    bool available() { return _begun; }
    void nextSample() {}
    uint16_t check() { return _begun ? 1 : 0; }
    bool safeCheck(uint8_t maxWait = 250) { (void)maxWait; return _begun; }

    float readTemperature() { return _temperature; }
    float readTemperatureF() { return _temperature * 9.0f / 5.0f + 32.0f; }

    void shutDown() { printf("[esp32emu] MAX30105: shutdown\n"); }
    void wakeUp() { printf("[esp32emu] MAX30105: wakeup\n"); }

    void softReset() {
        printf("[esp32emu] MAX30105: soft reset\n");
    }

    uint8_t getRevisionID() { return 0x03; }
    uint8_t getPartID() { return 0x15; }

    // Test helpers
    void test_setIR(uint32_t v) { _irValue = v; }
    void test_setRed(uint32_t v) { _redValue = v; }
    void test_setGreen(uint32_t v) { _greenValue = v; }
    void test_setTemperature(float t) { _temperature = t; }

private:
    bool _begun;
    uint32_t _irValue, _redValue, _greenValue;
    int _sampleRate;
    uint8_t _ledBrightness, _sampleAvg, _ledMode;
    int _adcRange, _pulseWidth;
    float _temperature;
};

// SpO2 algorithm helpers (simplified)
namespace spo2_algorithm {
    inline bool checkForBeat(uint32_t irValue) {
        return irValue > 50000;
    }
}
