#pragma once
#include "Arduino.h"
#include "SPI.h"

// esp32emu: MAX31865 RTD Temperature Sensor mock (PT100/PT1000)
class Adafruit_MAX31865 {
public:
    enum max31865_numwires_t { MAX31865_2WIRE = 0, MAX31865_3WIRE = 1, MAX31865_4WIRE = 0 };

    Adafruit_MAX31865(int8_t cs, int8_t mosi = -1, int8_t miso = -1, int8_t clk = -1)
        : _cs(cs), _mosi(mosi), _miso(miso), _clk(clk) {}

    bool begin(max31865_numwires_t wires = MAX31865_2WIRE) {
        _wires = wires;
        _initialized = true;
        Serial.println("[esp32emu] MAX31865: initialized");
        return true;
    }

    uint16_t readRTD() const { return _rtd; }

    float temperature(float nomR = 100.0f, float refR = 430.0f) const {
        (void)nomR; (void)refR;
        return _temperature;
    }

    uint8_t readFault() const { return _fault; }
    void clearFault() { _fault = 0; }

    void enableBias(bool enable) { _bias = enable; }
    void autoConvert(bool enable) { _autoConvert = enable; }
    void enable50Hz(bool enable) { _filter50Hz = enable; }

    // Fault thresholds
    void setThresholds(uint16_t low, uint16_t high) { _lowThresh = low; _highThresh = high; }

    // Test helpers
    void emu_setRTD(uint16_t rtd) { _rtd = rtd; }
    void emu_setTemperature(float t) { _temperature = t; }
    void emu_setFault(uint8_t fault) { _fault = fault; }
    bool emu_initialized() const { return _initialized; }
    bool emu_biasEnabled() const { return _bias; }

    // Fault bits
    static constexpr uint8_t MAX31865_FAULT_HIGHTHRESH = 0x80;
    static constexpr uint8_t MAX31865_FAULT_LOWTHRESH  = 0x40;
    static constexpr uint8_t MAX31865_FAULT_REFINLOW    = 0x20;
    static constexpr uint8_t MAX31865_FAULT_REFINHIGH   = 0x10;
    static constexpr uint8_t MAX31865_FAULT_RTDINLOW    = 0x08;
    static constexpr uint8_t MAX31865_FAULT_OVUV        = 0x04;

private:
    int8_t _cs, _mosi, _miso, _clk;
    max31865_numwires_t _wires = MAX31865_2WIRE;
    bool _initialized = false;
    uint16_t _rtd = 8192;       // ~100 ohm at ref=430
    float _temperature = 25.0f;
    uint8_t _fault = 0;
    bool _bias = false;
    bool _autoConvert = false;
    bool _filter50Hz = false;
    uint16_t _lowThresh = 0;
    uint16_t _highThresh = 0xFFFF;

};
