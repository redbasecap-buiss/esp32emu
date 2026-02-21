#pragma once
#include "Arduino.h"
#include <cstdio>
#include <cstdint>
#include <cmath>

// BMI270 6-axis IMU (SparkFun style) mock
class BMI270 {
public:
    BMI270() : _begun(false), _accelX(0), _accelY(0), _accelZ(9.81f),
               _gyroX(0), _gyroY(0), _gyroZ(0), _temp(25.0f) {}

    int8_t beginI2C(uint8_t addr = 0x68) {
        _addr = addr;
        _begun = true;
        printf("[esp32emu] BMI270: initialized I2C (addr=0x%02X)\n", addr);
        return 0; // BMI2_OK
    }

    int8_t beginSPI(uint8_t cs) {
        (void)cs;
        _begun = true;
        printf("[esp32emu] BMI270: initialized SPI (CS=%d)\n", cs);
        return 0;
    }

    int8_t getSensorData() {
        return _begun ? 0 : -1;
    }

    // Sensor data access
    float accX() const { return _accelX; }
    float accY() const { return _accelY; }
    float accZ() const { return _accelZ; }
    float gyrX() const { return _gyroX; }
    float gyrY() const { return _gyroY; }
    float gyrZ() const { return _gyroZ; }
    float temperature() const { return _temp; }

    // Config
    int8_t setAccelODR(uint8_t odr) { (void)odr; return 0; }
    int8_t setAccelRange(uint8_t range) { (void)range; return 0; }
    int8_t setGyroODR(uint8_t odr) { (void)odr; return 0; }
    int8_t setGyroRange(uint8_t range) { (void)range; return 0; }
    int8_t setAccelPowerMode(uint8_t mode) { (void)mode; return 0; }
    int8_t setGyroPowerMode(uint8_t mode) { (void)mode; return 0; }

    // Step counter
    int8_t enableStepCounter(bool en = true) { (void)en; return 0; }
    uint32_t getStepCount() { return _steps; }

    // Test helpers
    void test_setAccel(float x, float y, float z) { _accelX = x; _accelY = y; _accelZ = z; }
    void test_setGyro(float x, float y, float z) { _gyroX = x; _gyroY = y; _gyroZ = z; }
    void test_setTemp(float t) { _temp = t; }
    void test_setSteps(uint32_t s) { _steps = s; }

private:
    bool _begun;
    uint8_t _addr = 0x68;
    float _accelX, _accelY, _accelZ;
    float _gyroX, _gyroY, _gyroZ;
    float _temp;
    uint32_t _steps = 0;
};
