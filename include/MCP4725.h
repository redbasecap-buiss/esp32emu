#pragma once
#include "Arduino.h"
#include "Wire.h"

// esp32emu: MCP4725 12-bit DAC mock
class MCP4725 {
public:
    MCP4725() = default;

    bool begin(uint8_t addr = 0x60, TwoWire *wire = &Wire) {
        (void)wire;
        _addr = addr;
        _initialized = true;
        char buf[8]; snprintf(buf, sizeof(buf), "0x%02X", addr);
        Serial.println(String("[esp32emu] MCP4725: initialized at ") + buf);
        return true;
    }

    void setVoltage(uint16_t output, bool writeEEPROM = false) {
        _value = output & 0x0FFF;
        if (writeEEPROM) _eepromValue = _value;
    }

    uint16_t getValue() const { return _value; }

    // Convenience: set voltage as float (0.0 - Vref)
    void setVoltageFloat(float voltage, float vref = 3.3f) {
        if (voltage < 0) voltage = 0;
        if (voltage > vref) voltage = vref;
        _value = (uint16_t)((voltage / vref) * 4095.0f);
    }

    float getVoltageFloat(float vref = 3.3f) const {
        return ((float)_value / 4095.0f) * vref;
    }

    // Power-down modes
    enum PowerDownMode { PD_NORMAL = 0, PD_1K = 1, PD_100K = 2, PD_500K = 3 };
    void setPowerDown(PowerDownMode mode) { _pdMode = mode; }
    PowerDownMode getPowerDown() const { return _pdMode; }

    // Test helpers
    bool emu_initialized() const { return _initialized; }
    uint16_t emu_getEEPROM() const { return _eepromValue; }
    uint8_t emu_getAddress() const { return _addr; }

private:
    bool _initialized = false;
    uint8_t _addr = 0x60;
    uint16_t _value = 0;
    uint16_t _eepromValue = 0;
    PowerDownMode _pdMode = PD_NORMAL;
};
