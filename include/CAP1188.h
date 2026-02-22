#pragma once
// esp32emu — CAP1188 8-channel capacitive touch sensor mock

#include <cstdint>
#include <cstdio>

class CAP1188 {
public:
    CAP1188() = default;

    bool begin(uint8_t addr = 0x29, void* /*wire*/ = nullptr) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] CAP1188: initialized at 0x%02X\n", addr);
        return true;
    }

    uint8_t touched() {
        if (!initialized_) return 0;
        uint8_t t = touch_state_;
        touch_state_ = 0;  // clear on read
        return t;
    }

    bool isTouched(uint8_t channel) {
        if (!initialized_ || channel > 7) return false;
        bool t = (touch_state_ >> channel) & 1;
        return t;
    }

    void setSensitivity(uint8_t sensitivity) {
        sensitivity_ = sensitivity;
        fprintf(stderr, "[esp32emu] CAP1188: sensitivity set to %d\n", sensitivity);
    }

    uint8_t getSensitivity() const { return sensitivity_; }

    void enableMultiTouch(bool enable, uint8_t maxTouches = 4) {
        multi_touch_ = enable;
        max_touches_ = maxTouches;
        fprintf(stderr, "[esp32emu] CAP1188: multi-touch %s (max %d)\n",
                enable ? "enabled" : "disabled", maxTouches);
    }

    void setThreshold(uint8_t channel, uint8_t threshold) {
        if (channel > 7) return;
        thresholds_[channel] = threshold;
    }

    uint8_t readRegister(uint8_t reg) {
        (void)reg;
        return 0;
    }

    void writeRegister(uint8_t reg, uint8_t val) {
        (void)reg; (void)val;
    }

    void resetInterrupt() {
        fprintf(stderr, "[esp32emu] CAP1188: interrupt cleared\n");
    }

    uint8_t getProductID()    { return 0x50; }
    uint8_t getManufacturerID() { return 0x5D; }
    uint8_t getRevision()     { return 0x83; }

    // Test helpers
    void _setTouchState(uint8_t state)        { touch_state_ = state; }
    void _setChannelTouched(uint8_t ch, bool t) {
        if (ch > 7) return;
        if (t) touch_state_ |= (1 << ch);
        else   touch_state_ &= ~(1 << ch);
    }
    bool _isInitialized() const { return initialized_; }

private:
    uint8_t addr_ = 0x29;
    bool initialized_ = false;
    uint8_t touch_state_ = 0;
    uint8_t sensitivity_ = 5;
    bool multi_touch_ = false;
    uint8_t max_touches_ = 1;
    uint8_t thresholds_[8] = {64, 64, 64, 64, 64, 64, 64, 64};
};
