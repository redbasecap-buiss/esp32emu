#pragma once
// esp32emu — PCF8574 I2C I/O Expander mock
// Provides 8 GPIO pins via I2C (address 0x20-0x27 or 0x38-0x3F)

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

class PCF8574 {
public:
    PCF8574(uint8_t address = 0x20, TwoWire* wire = nullptr)
        : _address(address), _port(0xFF), _ddr(0x00), _wire(wire) {}

    bool begin(uint8_t sda = 255, uint8_t scl = 255) {
        (void)sda; (void)scl;
        _initialized = true;
        return true;
    }

    bool isConnected() const { return _initialized; }

    // Write entire 8-bit port
    bool write8(uint8_t value) {
        _port = value;
        return true;
    }

    // Read entire 8-bit port
    uint8_t read8() const {
        return _port;
    }

    // Write single pin (0-7)
    bool write(uint8_t pin, uint8_t value) {
        if (pin > 7) return false;
        if (value)
            _port |= (1 << pin);
        else
            _port &= ~(1 << pin);
        return true;
    }

    // Read single pin (0-7)
    uint8_t read(uint8_t pin) const {
        if (pin > 7) return 0;
        return (_port >> pin) & 1;
    }

    // Toggle single pin
    bool toggle(uint8_t pin) {
        if (pin > 7) return false;
        _port ^= (1 << pin);
        return true;
    }

    // Set pin mode (INPUT/OUTPUT for each pin via bitmask)
    void setButtonMask(uint8_t mask) { _ddr = mask; }

    // Invert read values
    bool setPolarity8(uint8_t mask) { _polarity = mask; return true; }

    uint8_t getAddress() const { return _address; }
    uint8_t lastError() const { return _error; }

    // --- Test helpers ---
    void test_setInput(uint8_t value) { _port = value; }
    uint8_t test_getOutput() const { return _port; }
    void test_setPin(uint8_t pin, bool high) {
        if (pin <= 7) {
            if (high) _port |= (1 << pin);
            else _port &= ~(1 << pin);
        }
    }

private:
    uint8_t _address;
    uint8_t _port;
    uint8_t _ddr;
    uint8_t _polarity = 0x00;
    uint8_t _error = 0;
    bool _initialized = false;
    TwoWire* _wire;  // NOLINT (stored for potential future use)
    void _suppress() { (void)_wire; }
};
