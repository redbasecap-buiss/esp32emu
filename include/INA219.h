#pragma once
// esp32emu — INA219 Current/Power Sensor mock
// Simulates the TI INA219 high-side current/power monitor over I2C.

#include <cstdint>
#include <cstdio>
#include <cmath>

class INA219 {
public:
    explicit INA219(uint8_t addr = 0x40) : addr_(addr) {}

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] INA219 initialized at 0x%02X\n", addr_);
        return true;
    }

    // Configure calibration. gain: 0=±40mV, 1=±80mV, 2=±160mV, 3=±320mV
    void setCalibration_32V_2A() { cal_mode_ = 0; shunt_lsb_ = 0.1f; }
    void setCalibration_32V_1A() { cal_mode_ = 1; shunt_lsb_ = 0.04f; }
    void setCalibration_16V_400mA() { cal_mode_ = 2; shunt_lsb_ = 0.01f; }

    // Read bus voltage in volts
    float getBusVoltage_V() const { return bus_voltage_; }

    // Read shunt voltage in millivolts
    float getShuntVoltage_mV() const { return shunt_voltage_mv_; }

    // Read current in milliamps
    float getCurrent_mA() const { return current_ma_; }

    // Read power in milliwatts
    float getPower_mW() const { return bus_voltage_ * current_ma_; }

    // Read raw bus voltage register
    int16_t getBusVoltage_raw() const { return (int16_t)(bus_voltage_ / 0.004f); }

    // Read raw shunt voltage register
    int16_t getShuntVoltage_raw() const { return (int16_t)(shunt_voltage_mv_ / 0.01f); }

    // Read raw current register
    int16_t getCurrent_raw() const { return (int16_t)(current_ma_ / shunt_lsb_); }

    // Read raw power register
    int16_t getPower_raw() const { return (int16_t)(getPower_mW() / (shunt_lsb_ * 20.0f)); }

    uint8_t getAddress() const { return addr_; }
    bool isInitialized() const { return initialized_; }

    // Power management
    void powerSave(bool on) { power_save_ = on; }
    bool isPowerSave() const { return power_save_; }

    // ── Test helpers ──

    void setBusVoltage(float volts) { bus_voltage_ = volts; }
    void setShuntVoltage(float millivolts) { shunt_voltage_mv_ = millivolts; }
    void setCurrent(float milliamps) { current_ma_ = milliamps; }

    // Set all readings at once (shunt voltage auto-calculated from current and shunt resistor)
    void setReadings(float busV, float currentMA, float shuntOhms = 0.1f) {
        bus_voltage_ = busV;
        current_ma_ = currentMA;
        shunt_voltage_mv_ = currentMA * shuntOhms;
    }

private:
    uint8_t addr_;
    bool initialized_ = false;
    bool power_save_ = false;
    int cal_mode_ = 0;
    float shunt_lsb_ = 0.1f;

    float bus_voltage_ = 5.0f;       // Default: 5V bus
    float shunt_voltage_mv_ = 10.0f; // Default: 10mV shunt
    float current_ma_ = 100.0f;      // Default: 100mA
};
