#pragma once
// esp32emu: MPU6050 accelerometer/gyroscope mock
#include <cstdint>
#include <cmath>

class MPU6050 {
public:
    MPU6050(uint8_t addr = 0x68) : _addr(addr), _begun(false),
        _ax(0), _ay(0), _az(16384),  // 1g on Z-axis at ±2g range
        _gx(0), _gy(0), _gz(0),
        _temp(25.0f) {}

    bool begin() { _begun = true; return true; }
    bool begin(uint8_t /*sda*/, uint8_t /*scl*/) { _begun = true; return true; }

    void getMotion6(int16_t* ax, int16_t* ay, int16_t* az,
                    int16_t* gx, int16_t* gy, int16_t* gz) {
        if (ax) *ax = _ax;
        if (ay) *ay = _ay;
        if (az) *az = _az;
        if (gx) *gx = _gx;
        if (gy) *gy = _gy;
        if (gz) *gz = _gz;
    }

    void getAcceleration(int16_t* ax, int16_t* ay, int16_t* az) {
        if (ax) *ax = _ax;
        if (ay) *ay = _ay;
        if (az) *az = _az;
    }

    void getRotation(int16_t* gx, int16_t* gy, int16_t* gz) {
        if (gx) *gx = _gx;
        if (gy) *gy = _gy;
        if (gz) *gz = _gz;
    }

    int16_t getAccelerationX() { return _ax; }
    int16_t getAccelerationY() { return _ay; }
    int16_t getAccelerationZ() { return _az; }
    int16_t getRotationX() { return _gx; }
    int16_t getRotationY() { return _gy; }
    int16_t getRotationZ() { return _gz; }

    float getTemperature() { return _temp; }

    // Convert raw accel to g (±2g range: 16384 LSB/g)
    float getAccelG_X() { return _ax / 16384.0f; }
    float getAccelG_Y() { return _ay / 16384.0f; }
    float getAccelG_Z() { return _az / 16384.0f; }

    // Convert raw gyro to deg/s (±250°/s range: 131 LSB/°/s)
    float getGyroDPS_X() { return _gx / 131.0f; }
    float getGyroDPS_Y() { return _gy / 131.0f; }
    float getGyroDPS_Z() { return _gz / 131.0f; }

    // Tilt angle from accelerometer (degrees)
    float getPitch() { return atan2(_ax, sqrt((float)_ay*_ay + (float)_az*_az)) * 180.0f / M_PI; }
    float getRoll()  { return atan2(_ay, sqrt((float)_ax*_ax + (float)_az*_az)) * 180.0f / M_PI; }

    void setFullScaleAccelRange(uint8_t range) { _accelRange = range; }
    void setFullScaleGyroRange(uint8_t range)  { _gyroRange = range; }

    // Test helpers
    void setAcceleration(int16_t ax, int16_t ay, int16_t az) { _ax = ax; _ay = ay; _az = az; }
    void setRotation(int16_t gx, int16_t gy, int16_t gz) { _gx = gx; _gy = gy; _gz = gz; }
    void setTemperature(float t) { _temp = t; }
    uint8_t getAddress() const { return _addr; }
    bool isBegun() const { return _begun; }

private:
    uint8_t _addr;
    bool _begun;
    int16_t _ax, _ay, _az;
    int16_t _gx, _gy, _gz;
    float _temp;
    uint8_t _accelRange = 0;
    uint8_t _gyroRange = 0;
};
