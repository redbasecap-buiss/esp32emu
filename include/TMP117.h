#pragma once
// esp32emu — TMP117 high-precision I2C temperature sensor mock

#include <cstdint>
#include <cstdio>
#include <cmath>

class TMP117 {
public:
    TMP117() = default;

    bool begin(uint8_t addr = 0x48, void* /*wire*/ = nullptr) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] TMP117: initialized at 0x%02X\n", addr);
        return true;
    }

    float readTemperature() {
        if (!initialized_) return NAN;
        data_ready_ = false;
        return temperature_;
    }

    float readTempC() { return readTemperature(); }
    float readTempF() { return readTemperature() * 9.0f / 5.0f + 32.0f; }

    bool dataReady() {
        if (!initialized_) return false;
        return data_ready_;
    }

    void setConversionMode(uint8_t mode) {
        conv_mode_ = mode;
        fprintf(stderr, "[esp32emu] TMP117: conversion mode set to %d\n", mode);
    }

    void setConversionCycleTime(uint8_t cycle) {
        conv_cycle_ = cycle;
        fprintf(stderr, "[esp32emu] TMP117: conversion cycle time set to %d\n", cycle);
    }

    void setAveraging(uint8_t avg) {
        averaging_ = avg;
        fprintf(stderr, "[esp32emu] TMP117: averaging set to %d\n", avg);
    }

    void setHighLimit(float limit) { high_limit_ = limit; }
    void setLowLimit(float limit)  { low_limit_ = limit; }
    float getHighLimit() const { return high_limit_; }
    float getLowLimit() const  { return low_limit_; }

    bool getHighAlert() const {
        return initialized_ && temperature_ > high_limit_;
    }

    bool getLowAlert() const {
        return initialized_ && temperature_ < low_limit_;
    }

    void softReset() {
        fprintf(stderr, "[esp32emu] TMP117: soft reset\n");
    }

    uint16_t getDeviceID() { return 0x0117; }

    // Test helpers
    void _setTemperature(float t) { temperature_ = t; data_ready_ = true; }
    void _setDataReady(bool r)    { data_ready_ = r; }
    bool _isInitialized() const   { return initialized_; }
    uint8_t _getAddr() const      { return addr_; }

private:
    uint8_t addr_ = 0x48;
    bool initialized_ = false;
    bool data_ready_ = true;
    float temperature_ = 22.5f;
    float high_limit_ = 85.0f;
    float low_limit_ = -40.0f;
    uint8_t conv_mode_ = 0;
    uint8_t conv_cycle_ = 0;
    uint8_t averaging_ = 0;
};
