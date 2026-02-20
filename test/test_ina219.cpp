#include "INA219.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Initialization
    {
        INA219 sensor(0x40);
        assert(!sensor.isInitialized());
        assert(sensor.begin());
        assert(sensor.isInitialized());
        assert(sensor.getAddress() == 0x40);
    }

    // Test 2: Default readings
    {
        INA219 sensor;
        sensor.begin();
        assert(near(sensor.getBusVoltage_V(), 5.0f));
        assert(near(sensor.getCurrent_mA(), 100.0f));
        assert(near(sensor.getPower_mW(), 500.0f));
    }

    // Test 3: Set custom readings
    {
        INA219 sensor;
        sensor.begin();
        sensor.setReadings(12.0f, 500.0f, 0.1f);

        assert(near(sensor.getBusVoltage_V(), 12.0f));
        assert(near(sensor.getCurrent_mA(), 500.0f));
        assert(near(sensor.getShuntVoltage_mV(), 50.0f));
        assert(near(sensor.getPower_mW(), 6000.0f));
    }

    // Test 4: Calibration modes
    {
        INA219 sensor;
        sensor.begin();
        sensor.setCalibration_32V_2A();
        sensor.setCalibration_32V_1A();
        sensor.setCalibration_16V_400mA();
        // Just verify no crash
    }

    // Test 5: Power save
    {
        INA219 sensor;
        sensor.begin();
        assert(!sensor.isPowerSave());
        sensor.powerSave(true);
        assert(sensor.isPowerSave());
    }

    // Test 6: Raw register values
    {
        INA219 sensor;
        sensor.begin();
        sensor.setBusVoltage(3.3f);
        int16_t raw = sensor.getBusVoltage_raw();
        assert(raw == (int16_t)(3.3f / 0.004f));
    }

    // Test 7: Individual setters
    {
        INA219 sensor;
        sensor.begin();
        sensor.setBusVoltage(24.0f);
        sensor.setCurrent(1500.0f);
        sensor.setShuntVoltage(150.0f);

        assert(near(sensor.getBusVoltage_V(), 24.0f));
        assert(near(sensor.getCurrent_mA(), 1500.0f));
        assert(near(sensor.getShuntVoltage_mV(), 150.0f));
        assert(near(sensor.getPower_mW(), 36000.0f));
    }

    // Test 8: Custom I2C address
    {
        INA219 sensor(0x41);
        sensor.begin();
        assert(sensor.getAddress() == 0x41);
    }

    printf("test_ina219: all assertions passed\n");
    return 0;
}
