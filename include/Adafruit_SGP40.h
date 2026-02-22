// esp32emu — Adafruit_SGP40 mock: VOC Index sensor (I2C)
#pragma once
#include <cstdint>
#include <cstdio>

class Adafruit_SGP40 {
public:
    Adafruit_SGP40() : _begun(false), _rawVOC(26000), _vocIndex(100) {}

    bool begin(uint8_t addr = 0x59, void* wire = nullptr) {
        (void)addr; (void)wire;
        _begun = true;
        printf("[esp32emu] SGP40: initialized (I2C 0x%02X)\n", addr);
        return true;
    }

    uint16_t measureRaw(float temperature = 25.0f, float humidity = 50.0f) {
        (void)temperature; (void)humidity;
        return _rawVOC;
    }

    int32_t measureVocIndex(float temperature = 25.0f, float humidity = 50.0f) {
        (void)temperature; (void)humidity;
        return _vocIndex;
    }

    bool selfTest() { return _begun; }

    void heaterOff() {
        if (_begun) printf("[esp32emu] SGP40: heater off\n");
    }

    // --- Test helpers ---
    void test_setRawVOC(uint16_t raw) { _rawVOC = raw; }
    void test_setVocIndex(int32_t idx) { _vocIndex = idx; }
    uint16_t test_getRawVOC() const { return _rawVOC; }
    int32_t test_getVocIndex() const { return _vocIndex; }
    bool test_isBegun() const { return _begun; }

private:
    bool _begun;
    uint16_t _rawVOC;
    int32_t _vocIndex;
};
