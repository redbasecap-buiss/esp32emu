#pragma once
#include "Arduino.h"
#include "Wire.h"

// esp32emu: SI7021 Temperature & Humidity Sensor mock
class Adafruit_SI7021 {
public:
    Adafruit_SI7021() = default;

    bool begin(TwoWire *wire = &Wire) {
        (void)wire;
        _initialized = true;
        Serial.println("[esp32emu] SI7021: initialized");
        return true;
    }

    float readTemperature() const { return _temperature; }
    float readHumidity() const { return _humidity; }

    // Resolution: 0=12bit RH/14bit Temp, 1=8/12, 2=10/13, 3=11/11
    void setResolution(uint8_t res) { _resolution = res; }
    uint8_t getResolution() const { return _resolution; }

    // Heater for condensation removal
    void heater(bool enable) { _heater = enable; }
    bool isHeaterEnabled() const { return _heater; }

    // Device info
    uint8_t getRevision() const { return 0x20; } // SI7021
    uint32_t sernum_a = 0x12345678;
    uint32_t sernum_b = 0xABCDEF00;

    // Test helpers
    void emu_setTemperature(float t) { _temperature = t; }
    void emu_setHumidity(float h) { _humidity = h; }
    bool emu_initialized() const { return _initialized; }

private:
    bool _initialized = false;
    float _temperature = 22.5f;
    float _humidity = 45.0f;
    uint8_t _resolution = 0;
    bool _heater = false;
};
