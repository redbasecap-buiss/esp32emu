// Adafruit_LC709203F.h — LC709203F battery fuel gauge mock for esp32emu
#pragma once
#include <cstdint>
#include <cstdio>

typedef enum {
    LC709203F_APA_100MAH  = 0x08,
    LC709203F_APA_200MAH  = 0x0B,
    LC709203F_APA_500MAH  = 0x10,
    LC709203F_APA_1000MAH = 0x19,
    LC709203F_APA_2000MAH = 0x2D,
    LC709203F_APA_3000MAH = 0x36
} lc709203_apa_t;

typedef enum {
    LC709203F_POWER_OPERATE = 1,
    LC709203F_POWER_SLEEP   = 2
} lc709203_powermode_t;

typedef enum {
    LC709203F_TEMP_I2C      = 0,
    LC709203F_TEMP_THERMISTOR = 1
} lc709203_tempmode_t;

class Adafruit_LC709203F {
public:
    Adafruit_LC709203F() = default;

    bool begin() {
        _begun = true;
        printf("[esp32emu] LC709203F: battery gauge initialized\n");
        return true;
    }

    bool initRSOC() {
        printf("[esp32emu] LC709203F: RSOC initialized\n");
        return true;
    }

    float cellVoltage() const { return _voltage; }
    float cellPercent() const { return _soc; }

    uint16_t getICversion() const { return 0x2717; }

    void setPackSize(lc709203_apa_t apa) { _apa = apa; }
    lc709203_apa_t getPackSize() const { return _apa; }

    void setPackAPA(uint8_t apa) { _apaRaw = apa; }
    uint8_t getPackAPA() const { return _apaRaw; }

    void setPowerMode(lc709203_powermode_t mode) {
        _powerMode = mode;
        printf("[esp32emu] LC709203F: power mode=%s\n",
               mode == LC709203F_POWER_SLEEP ? "sleep" : "operate");
    }
    lc709203_powermode_t getPowerMode() const { return _powerMode; }

    void setTemperatureMode(lc709203_tempmode_t mode) { _tempMode = mode; }
    lc709203_tempmode_t getTemperatureMode() const { return _tempMode; }

    void setCellCapacity(lc709203_apa_t apa) { _apa = apa; }

    void setAlarmRSOC(uint8_t pct) { _alarmRSOC = pct; }
    uint8_t getAlarmRSOC() const { return _alarmRSOC; }

    void setAlarmVoltage(float v) { _alarmVoltage = v; }
    float getAlarmVoltage() const { return _alarmVoltage; }

    void setCellProfile(uint16_t p) { _profile = p; }
    uint16_t getCellProfile() const { return _profile; }

    float getThermistorB() const { return _thermB; }
    void setThermistorB(float b) { _thermB = b; }

    // Test helpers
    void setTestVoltage(float v) { _voltage = v; }
    void setTestSOC(float soc) { _soc = soc; }

private:
    bool _begun = false;
    float _voltage = 3.8f, _soc = 80.0f;
    lc709203_apa_t _apa = LC709203F_APA_1000MAH;
    uint8_t _apaRaw = 0x19;
    lc709203_powermode_t _powerMode = LC709203F_POWER_OPERATE;
    lc709203_tempmode_t _tempMode = LC709203F_TEMP_I2C;
    uint8_t _alarmRSOC = 0;
    float _alarmVoltage = 0.0f;
    uint16_t _profile = 1;
    float _thermB = 3380.0f;
};
