#pragma once
// esp32emu — SGP30 Air Quality Sensor mock (TVOC + eCO2)

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

class SGP30 {
public:
    SGP30() = default;

    bool begin(TwoWire* /*wire*/ = nullptr) {
        _initialized = true;
        return true;
    }

    // Trigger a measurement
    bool IAQmeasure() {
        if (!_initialized) return false;
        return true;
    }

    // Trigger raw measurement
    bool IAQmeasureRaw() {
        if (!_initialized) return false;
        return true;
    }

    // Get serial number
    bool getSerialNumber(uint16_t serial[3]) {
        serial[0] = 0x0000;
        serial[1] = 0x0048;
        serial[2] = 0xB6A0;
        return true;
    }

    // Set humidity compensation (absolute humidity in mg/m³, 8.8 fixed point)
    void setHumidity(uint32_t abs_humidity) {
        _abs_humidity = abs_humidity;
    }

    // Get/set baseline for calibration
    bool getIAQBaseline(uint16_t* eco2_base, uint16_t* tvoc_base) {
        *eco2_base = _eco2_baseline;
        *tvoc_base = _tvoc_baseline;
        return true;
    }

    bool setIAQBaseline(uint16_t eco2_base, uint16_t tvoc_base) {
        _eco2_baseline = eco2_base;
        _tvoc_baseline = tvoc_base;
        return true;
    }

    // Public measurement results
    uint16_t TVOC = 42;          // ppb
    uint16_t eCO2 = 400;         // ppm
    uint16_t rawH2 = 13000;      // raw H2 signal
    uint16_t rawEthanol = 18000; // raw Ethanol signal

    // --- Test helpers ---
    void test_setTVOC(uint16_t tvoc) { TVOC = tvoc; }
    void test_seteCO2(uint16_t eco2) { eCO2 = eco2; }
    void test_setRaw(uint16_t h2, uint16_t ethanol) { rawH2 = h2; rawEthanol = ethanol; }
    bool test_isInitialized() const { return _initialized; }

private:
    bool _initialized = false;
    uint32_t _abs_humidity = 0;
    uint16_t _eco2_baseline = 0x8A12;
    uint16_t _tvoc_baseline = 0x9134;
};
