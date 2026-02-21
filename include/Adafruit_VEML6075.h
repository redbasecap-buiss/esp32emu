#pragma once
#include "Arduino.h"
#include "Wire.h"

// esp32emu: VEML6075 UV Sensor mock
class Adafruit_VEML6075 {
public:
    Adafruit_VEML6075() = default;

    bool begin(TwoWire *wire = &Wire) {
        (void)wire;
        _initialized = true;
        Serial.println("[esp32emu] VEML6075: initialized");
        return true;
    }

    enum veml6075_integtime { VEML6075_50MS, VEML6075_100MS, VEML6075_200MS, VEML6075_400MS, VEML6075_800MS };

    void setIntegrationTime(veml6075_integtime it) { _it = it; }
    veml6075_integtime getIntegrationTime() const { return _it; }
    void setHighDynamic(bool hd) { _highDynamic = hd; }
    void setForcedMode(bool forced) { _forcedMode = forced; }
    void setCoefficients(float a, float b, float c, float d, float uvaresp, float uvbresp) {
        _a = a; _b = b; _c = c; _d = d; _uvaResp = uvaresp; _uvbResp = uvbresp;
    }

    float readUVA() const { return _uva; }
    float readUVB() const { return _uvb; }
    float readUVI() const { return _uvi; }

    // Test helpers
    void emu_setUVA(float uva) { _uva = uva; }
    void emu_setUVB(float uvb) { _uvb = uvb; }
    void emu_setUVI(float uvi) { _uvi = uvi; }
    bool emu_initialized() const { return _initialized; }

private:
    bool _initialized = false;
    float _uva = 15.0f;
    float _uvb = 8.0f;
    float _uvi = 3.5f;
    veml6075_integtime _it = VEML6075_100MS;
    bool _highDynamic = false;
    bool _forcedMode = false;
    float _a = 2.22f, _b = 1.33f, _c = 2.95f, _d = 1.74f;
    float _uvaResp = 0.001461f, _uvbResp = 0.002591f;
};
