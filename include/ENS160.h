#pragma once
// esp32emu — ENS160 digital multi-gas sensor mock (TVOC, eCO2, AQI)

#include <cstdint>
#include <cstdio>

class ENS160 {
public:
    ENS160() = default;

    bool begin(uint8_t addr = 0x53, void* /*wire*/ = nullptr) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] ENS160: initialized at 0x%02X\n", addr);
        return true;
    }

    bool setMode(uint8_t mode) {
        if (!initialized_) return false;
        mode_ = mode;
        fprintf(stderr, "[esp32emu] ENS160: mode set to %d\n", mode);
        return true;
    }

    bool available() { return initialized_ && data_ready_; }

    uint8_t getAQI() {
        if (!initialized_) return 0;
        return aqi_;
    }

    uint16_t getTVOC() {
        if (!initialized_) return 0;
        return tvoc_;
    }

    uint16_t geteCO2() {
        if (!initialized_) return 0;
        return eco2_;
    }

    uint32_t getHP0() { return hp0_; }
    uint32_t getHP1() { return hp1_; }
    uint32_t getHP2() { return hp2_; }
    uint32_t getHP3() { return hp3_; }

    bool setTempCompensation(float temp) {
        comp_temp_ = temp;
        fprintf(stderr, "[esp32emu] ENS160: temp compensation set to %.1f°C\n", temp);
        return true;
    }

    bool setRHCompensation(float rh) {
        comp_rh_ = rh;
        fprintf(stderr, "[esp32emu] ENS160: RH compensation set to %.1f%%\n", rh);
        return true;
    }

    uint8_t getStatus() { return status_; }
    uint16_t getFirmwareVersion() { return 0x0310; }

    void reset() {
        fprintf(stderr, "[esp32emu] ENS160: reset\n");
    }

    // Operating modes
    static constexpr uint8_t MODE_DEEP_SLEEP = 0;
    static constexpr uint8_t MODE_IDLE       = 1;
    static constexpr uint8_t MODE_STANDARD   = 2;

    // AQI levels
    static constexpr uint8_t AQI_EXCELLENT   = 1;
    static constexpr uint8_t AQI_GOOD        = 2;
    static constexpr uint8_t AQI_MODERATE    = 3;
    static constexpr uint8_t AQI_POOR        = 4;
    static constexpr uint8_t AQI_UNHEALTHY   = 5;

    // Test helpers
    void _setAQI(uint8_t v)      { aqi_ = v; }
    void _setTVOC(uint16_t v)    { tvoc_ = v; }
    void _seteCO2(uint16_t v)    { eco2_ = v; }
    void _setDataReady(bool r)   { data_ready_ = r; }
    void _setStatus(uint8_t s)   { status_ = s; }
    void _setHP(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3) {
        hp0_ = h0; hp1_ = h1; hp2_ = h2; hp3_ = h3;
    }
    bool _isInitialized() const  { return initialized_; }

private:
    uint8_t addr_ = 0x53;
    bool initialized_ = false;
    bool data_ready_ = true;
    uint8_t mode_ = 0;
    uint8_t aqi_ = 1;
    uint16_t tvoc_ = 50;
    uint16_t eco2_ = 400;
    uint8_t status_ = 0;
    float comp_temp_ = 25.0f;
    float comp_rh_ = 50.0f;
    uint32_t hp0_ = 0, hp1_ = 0, hp2_ = 0, hp3_ = 0;
};
