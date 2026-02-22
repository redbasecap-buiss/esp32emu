// MAX17048.h — MAX17048 LiPo fuel gauge mock for esp32emu
#pragma once
#include "Wire.h"
#include <cstdint>
#include <cstdio>

class MAX17048 {
public:
    MAX17048() : _voltage(3.7f), _soc(75.0f), _rate(0.0f),
                 _alertThreshold(4), _hibernate(false), _sleeping(false),
                 _alertFlag(false), _resetFlag(false), _begun(false) {}

    bool begin(TwoWire& wire = Wire) {
        (void)wire;
        _begun = true;
        printf("[esp32emu] MAX17048: initialized (V=%.2fV, SOC=%.1f%%)\n", _voltage, _soc);
        return true;
    }

    bool isConnected() const { return _begun; }

    // Voltage in volts
    float cellVoltage() const { return _voltage; }
    // State of charge in percent
    float cellPercent() const { return _soc; }
    // Charge/discharge rate in %/hr
    float chargeRate() const { return _rate; }

    // Version register
    uint16_t getVersion() const { return 0x0013; }

    // Alert threshold (1-32%)
    void setAlertThreshold(uint8_t pct) {
        _alertThreshold = (pct > 32) ? 32 : (pct < 1 ? 1 : pct);
        printf("[esp32emu] MAX17048: alert threshold=%d%%\n", _alertThreshold);
    }
    uint8_t getAlertThreshold() const { return _alertThreshold; }

    // Alert flag
    bool isActiveAlert() const { return _alertFlag || (_soc <= _alertThreshold); }
    void clearAlert() { _alertFlag = false; }

    // Reset indicator
    bool isReset() const { return _resetFlag; }
    void clearReset() { _resetFlag = false; }

    // Hibernate mode
    void enableHibernate() { _hibernate = true; printf("[esp32emu] MAX17048: hibernate enabled\n"); }
    void disableHibernate() { _hibernate = false; printf("[esp32emu] MAX17048: hibernate disabled\n"); }
    bool isHibernating() const { return _hibernate; }

    // Sleep mode
    void enableSleep() { _sleeping = true; printf("[esp32emu] MAX17048: sleep mode\n"); }
    void disableSleep() { _sleeping = false; printf("[esp32emu] MAX17048: wake up\n"); }
    bool isSleeping() const { return _sleeping; }

    // Quick start
    void quickStart() { printf("[esp32emu] MAX17048: quick start\n"); }

    // Reset
    void reset() {
        _resetFlag = true;
        printf("[esp32emu] MAX17048: reset\n");
    }

    // Test helpers
    void setTestVoltage(float v) { _voltage = v; }
    void setTestSOC(float soc) { _soc = soc; }
    void setTestRate(float rate) { _rate = rate; }
    void setTestAlert(bool a) { _alertFlag = a; }

private:
    float _voltage, _soc, _rate;
    uint8_t _alertThreshold;
    bool _hibernate, _sleeping, _alertFlag, _resetFlag, _begun;
};
