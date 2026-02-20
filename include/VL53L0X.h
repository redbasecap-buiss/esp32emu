#pragma once
// esp32emu: VL53L0X Time-of-Flight distance sensor mock
#include <cstdint>

class VL53L0X {
public:
    VL53L0X() : _addr(0x29), _begun(false), _distance(100),
        _signalRate(5000), _ambientRate(200), _spadCount(8),
        _continuous(false), _timeout(500), _measTimingBudget(33000) {}

    bool begin(uint8_t addr = 0x29) {
        _addr = addr;
        _begun = true;
        return true;
    }

    uint16_t readRangeSingleMillimeters() { return _distance; }
    uint16_t readRangeContinuousMillimeters() { return _distance; }

    void startContinuous(uint32_t /*period_ms*/ = 0) { _continuous = true; }
    void stopContinuous() { _continuous = false; }
    bool dataReady() { return true; }
    uint16_t read() { return _distance; }

    bool timeoutOccurred() { return false; }
    void setTimeout(uint16_t t) { _timeout = t; }
    uint16_t getTimeout() { return _timeout; }

    bool setMeasurementTimingBudget(uint32_t budget_us) {
        if (budget_us < 20000) return false;
        _measTimingBudget = budget_us;
        return true;
    }
    uint32_t getMeasurementTimingBudget() { return _measTimingBudget; }

    void setAddress(uint8_t addr) { _addr = addr; }
    uint8_t getAddress() const { return _addr; }

    // Signal quality
    uint16_t getSignalRateReturn() { return _signalRate; }
    uint16_t getAmbientRateReturn() { return _ambientRate; }
    uint8_t getSpadCount() { return _spadCount; }

    enum RangeStatus { RANGE_VALID = 0, SIGMA_FAIL = 1, SIGNAL_FAIL = 2,
                       MIN_RANGE_FAIL = 3, PHASE_FAIL = 4, HW_FAIL = 5 };
    RangeStatus getRangeStatus() { return RANGE_VALID; }

    // Long range mode
    bool setSignalRateLimit(float limit_mcps) { _sigRateLimit = limit_mcps; return true; }
    float getSignalRateLimit() { return _sigRateLimit; }
    void setVcselPulsePeriod(uint8_t type, uint8_t period) {
        if (type == 0) _preRangePeriod = period;
        else _finalRangePeriod = period;
    }

    // Test helpers
    void setDistance(uint16_t mm) { _distance = mm; }
    void setSignalRate(uint16_t rate) { _signalRate = rate; }
    void setAmbientRate(uint16_t rate) { _ambientRate = rate; }
    bool isBegun() const { return _begun; }
    bool isContinuous() const { return _continuous; }

private:
    uint8_t _addr;
    bool _begun;
    uint16_t _distance;
    uint16_t _signalRate;
    uint16_t _ambientRate;
    uint8_t _spadCount;
    bool _continuous;
    uint16_t _timeout;
    uint32_t _measTimingBudget;
    float _sigRateLimit = 0.25f;
    uint8_t _preRangePeriod = 14;
    uint8_t _finalRangePeriod = 10;
};
