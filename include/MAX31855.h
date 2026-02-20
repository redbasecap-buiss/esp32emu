#pragma once
// esp32emu: MAX31855 thermocouple sensor mock
#include <cstdint>
#include <cmath>

class Adafruit_MAX31855 {
public:
    Adafruit_MAX31855(int8_t clk, int8_t cs, int8_t miso)
        : _clk(clk), _cs(cs), _miso(miso), _begun(false),
          _thermoC(25.0), _internalC(25.0), _fault(0) {}

    // SPI constructor
    Adafruit_MAX31855(int8_t cs)
        : _clk(-1), _cs(cs), _miso(-1), _begun(false),
          _thermoC(25.0), _internalC(25.0), _fault(0) {}

    bool begin() { _begun = true; return true; }

    double readCelsius() {
        if (_fault) return NAN;
        return _thermoC;
    }

    double readFahrenheit() {
        double c = readCelsius();
        if (std::isnan(c)) return NAN;
        return c * 9.0 / 5.0 + 32.0;
    }

    double readInternal() { return _internalC; }

    uint8_t readError() { return _fault; }

    uint32_t readRaw32() {
        uint32_t raw = 0;
        // Encode thermocouple temp (bits 31-18, 14 bits, 0.25°C/LSB)
        int16_t tc = (int16_t)(_thermoC * 4.0);
        raw |= ((uint32_t)(tc & 0x3FFF)) << 18;
        // Encode internal temp (bits 15-4, 12 bits, 0.0625°C/LSB)
        int16_t ic = (int16_t)(_internalC * 16.0);
        raw |= ((uint32_t)(ic & 0x0FFF)) << 4;
        // Fault bits
        raw |= (_fault & 0x07);
        if (_fault) raw |= (1u << 16); // fault flag
        return raw;
    }

    // Fault constants
    static constexpr uint8_t FAULT_OPEN   = 0x01;
    static constexpr uint8_t FAULT_SHORT_GND = 0x02;
    static constexpr uint8_t FAULT_SHORT_VCC = 0x04;

    // Test helpers
    void setTemperature(double thermoC) { _thermoC = thermoC; }
    void setInternalTemperature(double intC) { _internalC = intC; }
    void setFault(uint8_t fault) { _fault = fault; }
    void clearFault() { _fault = 0; }
    bool isBegun() const { return _begun; }

private:
    int8_t _clk, _cs, _miso;
    bool _begun;
    double _thermoC;
    double _internalC;
    uint8_t _fault;
};
