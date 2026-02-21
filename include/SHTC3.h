#pragma once
// esp32emu — SHTC3 humidity/temperature sensor mock (SparkFun-compatible API)
// Compact, low-power sensor. Provides temperature (°C/°F), humidity (%), raw data.

#include <cstdint>
#include <cstdio>

namespace esp32emu {
namespace shtc3 {

struct SHTC3State {
    float temperature = 22.5f; // °C
    float humidity = 45.0f;    // %RH
    bool sleeping = false;
    bool initialized = false;

    static SHTC3State& instance() {
        static SHTC3State s;
        return s;
    }
    void reset() { *this = SHTC3State(); }
};

} // namespace shtc3
} // namespace esp32emu

typedef enum {
    SHTC3_Status_Nominal = 0,
    SHTC3_Status_Error   = 1,
    SHTC3_Status_CRC_Fail = 2
} SHTC3_Status_TypeDef;

class SHTC3_RawData {
public:
    float toDegC() const { return _tempC; }
    float toDegF() const { return _tempC * 9.0f / 5.0f + 32.0f; }
    float toPercent() const { return _humidity; }
    SHTC3_Status_TypeDef status = SHTC3_Status_Nominal;
    float _tempC = 22.5f;
    float _humidity = 45.0f;
};

class SHTC3 {
public:
    SHTC3() {}

    SHTC3_Status_TypeDef begin() {
        auto& s = esp32emu::shtc3::SHTC3State::instance();
        s.initialized = true;
        printf("[esp32emu] SHTC3: initialized\n");
        return SHTC3_Status_Nominal;
    }

    SHTC3_Status_TypeDef update() {
        auto& s = esp32emu::shtc3::SHTC3State::instance();
        if (s.sleeping) return SHTC3_Status_Error;
        _lastData._tempC = s.temperature;
        _lastData._humidity = s.humidity;
        _lastData.status = SHTC3_Status_Nominal;
        return SHTC3_Status_Nominal;
    }

    SHTC3_Status_TypeDef sleep() {
        esp32emu::shtc3::SHTC3State::instance().sleeping = true;
        return SHTC3_Status_Nominal;
    }

    SHTC3_Status_TypeDef wake() {
        esp32emu::shtc3::SHTC3State::instance().sleeping = false;
        return SHTC3_Status_Nominal;
    }

    float toDegC() const { return _lastData.toDegC(); }
    float toDegF() const { return _lastData.toDegF(); }
    float toPercent() const { return _lastData.toPercent(); }

    SHTC3_RawData toRawData() const { return _lastData; }

    bool passIDcrc = true;
    bool lastStatus() const { return _lastData.status == SHTC3_Status_Nominal; }

private:
    SHTC3_RawData _lastData;
};

// Test helpers
namespace esp32emu_shtc3_test {
    inline void setTemperature(float t) {
        esp32emu::shtc3::SHTC3State::instance().temperature = t;
    }
    inline void setHumidity(float h) {
        esp32emu::shtc3::SHTC3State::instance().humidity = h;
    }
    inline void reset() {
        esp32emu::shtc3::SHTC3State::instance().reset();
    }
}
