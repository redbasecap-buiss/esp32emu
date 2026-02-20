#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Wire.h"

// MCP23017 — 16-bit I2C GPIO expander mock
// Two 8-bit ports (A and B), each pin configurable as input/output with pull-ups

#define MCP23017_ADDRESS 0x20

// Register addresses (BANK=0, default)
#define MCP23017_IODIRA   0x00
#define MCP23017_IODIRB   0x01
#define MCP23017_IPOLA    0x02
#define MCP23017_IPOLB    0x03
#define MCP23017_GPINTENA 0x04
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALA  0x06
#define MCP23017_DEFVALB  0x07
#define MCP23017_INTCONA  0x08
#define MCP23017_INTCONB  0x09
#define MCP23017_IOCONA   0x0A
#define MCP23017_IOCONB   0x0B
#define MCP23017_GPPUA    0x0C
#define MCP23017_GPPUB    0x0D
#define MCP23017_INTFA    0x0E
#define MCP23017_INTFB    0x0F
#define MCP23017_INTCAPA  0x10
#define MCP23017_INTCAPB  0x11
#define MCP23017_GPIOA    0x12
#define MCP23017_GPIOB    0x13
#define MCP23017_OLATA    0x14
#define MCP23017_OLATB    0x15

class MCP23017 {
public:
    MCP23017() { reset(); }

    bool begin(uint8_t addr = MCP23017_ADDRESS, TwoWire* wire = nullptr) {
        _addr = addr;
        _initialized = true;
        printf("[MCP23017] Initialized at 0x%02X\n", addr);
        return true;
    }

    // Pin mode: 0 = output, 1 = input (matches MCP23017 IODIR register)
    void pinMode(uint8_t pin, uint8_t mode) {
        if (pin > 15) return;
        if (mode == 1) { // INPUT
            _iodir |= (1 << pin);
        } else { // OUTPUT
            _iodir &= ~(1 << pin);
        }
    }

    void digitalWrite(uint8_t pin, uint8_t val) {
        if (pin > 15) return;
        if (val) {
            _gpio |= (1 << pin);
        } else {
            _gpio &= ~(1 << pin);
        }
    }

    uint8_t digitalRead(uint8_t pin) {
        if (pin > 15) return 0;
        return (_gpio >> pin) & 1;
    }

    // Write all 16 pins at once
    void writeGPIOAB(uint16_t val) { _gpio = val; }
    uint16_t readGPIOAB() { return _gpio; }

    // Write individual ports
    void writeGPIOA(uint8_t val) { _gpio = (_gpio & 0xFF00) | val; }
    void writeGPIOB(uint8_t val) { _gpio = (_gpio & 0x00FF) | ((uint16_t)val << 8); }
    uint8_t readGPIOA() { return _gpio & 0xFF; }
    uint8_t readGPIOB() { return (_gpio >> 8) & 0xFF; }

    // Pull-up configuration
    void setPullup(uint8_t pin, bool enabled) {
        if (pin > 15) return;
        if (enabled) {
            _pullup |= (1 << pin);
        } else {
            _pullup &= ~(1 << pin);
        }
    }

    void setPullupAB(uint16_t val) { _pullup = val; }
    uint16_t getPullupAB() { return _pullup; }

    // Interrupt configuration
    void setInterrupt(uint8_t pin, bool enabled) {
        if (pin > 15) return;
        if (enabled) {
            _gpinten |= (1 << pin);
        } else {
            _gpinten &= ~(1 << pin);
        }
    }

    uint16_t getInterruptEnabled() { return _gpinten; }

    // Polarity inversion
    void setPolarity(uint8_t pin, bool inverted) {
        if (pin > 15) return;
        if (inverted) {
            _ipol |= (1 << pin);
        } else {
            _ipol &= ~(1 << pin);
        }
    }

    // Test helpers
    uint16_t getIODIR() { return _iodir; }
    uint16_t getGPIO() { return _gpio; }
    bool isInitialized() { return _initialized; }

    // Inject pin state for testing input reads
    void test_setPin(uint8_t pin, bool val) {
        if (pin > 15) return;
        if (val) {
            _gpio |= (1 << pin);
        } else {
            _gpio &= ~(1 << pin);
        }
    }

    void reset() {
        _addr = MCP23017_ADDRESS;
        _initialized = false;
        _iodir = 0xFFFF;   // All inputs by default
        _gpio = 0x0000;
        _pullup = 0x0000;
        _gpinten = 0x0000;
        _ipol = 0x0000;
    }

private:
    uint8_t  _addr = MCP23017_ADDRESS;
    bool     _initialized = false;
    uint16_t _iodir = 0xFFFF;
    uint16_t _gpio = 0x0000;
    uint16_t _pullup = 0x0000;
    uint16_t _gpinten = 0x0000;
    uint16_t _ipol = 0x0000;
};
