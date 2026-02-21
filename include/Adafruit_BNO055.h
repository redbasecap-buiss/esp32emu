#pragma once
// esp32emu — Adafruit BNO055 9-DOF Absolute Orientation IMU mock
// Provides orientation (Euler angles), quaternion, accelerometer, gyroscope,
// magnetometer, linear acceleration, gravity vector, calibration status.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

#ifndef ESP_OK
#define ESP_OK 0
#endif

typedef enum {
    OPERATION_MODE_CONFIG   = 0x00,
    OPERATION_MODE_ACCONLY  = 0x01,
    OPERATION_MODE_MAGONLY  = 0x02,
    OPERATION_MODE_GYRONLY  = 0x03,
    OPERATION_MODE_ACCMAG  = 0x04,
    OPERATION_MODE_ACCGYRO = 0x05,
    OPERATION_MODE_MAGGYRO = 0x06,
    OPERATION_MODE_AMG     = 0x07,
    OPERATION_MODE_IMUPLUS = 0x08,
    OPERATION_MODE_COMPASS = 0x09,
    OPERATION_MODE_M4G     = 0x0A,
    OPERATION_MODE_NDOF_FMC_OFF = 0x0B,
    OPERATION_MODE_NDOF    = 0x0C
} adafruit_bno055_opmode_t;

typedef enum {
    VECTOR_ACCELEROMETER = 0x08,
    VECTOR_MAGNETOMETER  = 0x0E,
    VECTOR_GYROSCOPE     = 0x14,
    VECTOR_EULER         = 0x1A,
    VECTOR_LINEARACCEL   = 0x28,
    VECTOR_GRAVITY       = 0x2E
} adafruit_vector_type_t;

namespace esp32emu {
namespace bno055 {

struct Vec3 {
    double x = 0, y = 0, z = 0;
};

struct Quat {
    double w = 1, x = 0, y = 0, z = 0;
};

struct BNO055State {
    bool initialized = false;
    adafruit_bno055_opmode_t mode = OPERATION_MODE_CONFIG;
    Vec3 euler;       // heading, roll, pitch (degrees)
    Vec3 accel;       // m/s^2
    Vec3 gyro;        // rad/s
    Vec3 mag;         // uT
    Vec3 linearAccel; // m/s^2
    Vec3 gravity;     // m/s^2
    Quat quat;
    int8_t temp = 25;
    // Calibration: 0=uncalibrated, 3=fully calibrated
    uint8_t calSys = 3, calGyro = 3, calAccel = 3, calMag = 3;

    static BNO055State& instance() {
        static BNO055State s;
        return s;
    }
    void reset() { *this = BNO055State(); }
};

} // namespace bno055
} // namespace esp32emu

struct imu_vector_t {
    double x() const { return _x; }
    double y() const { return _y; }
    double z() const { return _z; }
    double _x = 0, _y = 0, _z = 0;
};

struct imu_quat_t {
    double w() const { return _w; }
    double x() const { return _x; }
    double y() const { return _y; }
    double z() const { return _z; }
    double _w = 1, _x = 0, _y = 0, _z = 0;
};

class Adafruit_BNO055 {
public:
    Adafruit_BNO055(int32_t sensorID = -1, uint8_t address = 0x28)
        : _sensorID(sensorID), _address(address) {}

    bool begin(adafruit_bno055_opmode_t mode = OPERATION_MODE_NDOF) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.initialized = true;
        s.mode = mode;
        s.gravity = {0, 0, 9.81};
        printf("[esp32emu] BNO055: initialized (addr=0x%02X, mode=0x%02X)\n", _address, mode);
        return true;
    }

    void setMode(adafruit_bno055_opmode_t mode) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.mode = mode;
    }

    adafruit_bno055_opmode_t getMode() {
        return esp32emu::bno055::BNO055State::instance().mode;
    }

    imu_vector_t getVector(adafruit_vector_type_t type) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        imu_vector_t v;
        switch (type) {
            case VECTOR_EULER:        v._x = s.euler.x; v._y = s.euler.y; v._z = s.euler.z; break;
            case VECTOR_ACCELEROMETER:v._x = s.accel.x; v._y = s.accel.y; v._z = s.accel.z; break;
            case VECTOR_GYROSCOPE:    v._x = s.gyro.x;  v._y = s.gyro.y;  v._z = s.gyro.z;  break;
            case VECTOR_MAGNETOMETER: v._x = s.mag.x;   v._y = s.mag.y;   v._z = s.mag.z;   break;
            case VECTOR_LINEARACCEL:  v._x = s.linearAccel.x; v._y = s.linearAccel.y; v._z = s.linearAccel.z; break;
            case VECTOR_GRAVITY:      v._x = s.gravity.x; v._y = s.gravity.y; v._z = s.gravity.z; break;
        }
        return v;
    }

    imu_quat_t getQuat() {
        auto& s = esp32emu::bno055::BNO055State::instance();
        imu_quat_t q;
        q._w = s.quat.w; q._x = s.quat.x; q._y = s.quat.y; q._z = s.quat.z;
        return q;
    }

    int8_t getTemp() {
        return esp32emu::bno055::BNO055State::instance().temp;
    }

    void getCalibration(uint8_t* sys, uint8_t* gyro, uint8_t* accel, uint8_t* mag) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        if (sys) *sys = s.calSys;
        if (gyro) *gyro = s.calGyro;
        if (accel) *accel = s.calAccel;
        if (mag) *mag = s.calMag;
    }

    bool isFullyCalibrated() {
        auto& s = esp32emu::bno055::BNO055State::instance();
        return s.calSys == 3 && s.calGyro == 3 && s.calAccel == 3 && s.calMag == 3;
    }

    bool getSensorOffsets(uint8_t* buf) {
        if (buf) memset(buf, 0, 22);
        return true;
    }

    void setSensorOffsets(const uint8_t* /*buf*/) {}

private:
    int32_t _sensorID;
    uint8_t _address;
};

// Test helpers
namespace esp32emu_bno055_test {
    inline void setEuler(double heading, double roll, double pitch) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.euler = {heading, roll, pitch};
    }
    inline void setAccel(double x, double y, double z) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.accel = {x, y, z};
    }
    inline void setGyro(double x, double y, double z) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.gyro = {x, y, z};
    }
    inline void setMag(double x, double y, double z) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.mag = {x, y, z};
    }
    inline void setQuat(double w, double x, double y, double z) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.quat = {w, x, y, z};
    }
    inline void setLinearAccel(double x, double y, double z) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.linearAccel = {x, y, z};
    }
    inline void setGravity(double x, double y, double z) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.gravity = {x, y, z};
    }
    inline void setTemp(int8_t t) {
        esp32emu::bno055::BNO055State::instance().temp = t;
    }
    inline void setCalibration(uint8_t sys, uint8_t gyro, uint8_t accel, uint8_t mag) {
        auto& s = esp32emu::bno055::BNO055State::instance();
        s.calSys = sys; s.calGyro = gyro; s.calAccel = accel; s.calMag = mag;
    }
    inline void reset() {
        esp32emu::bno055::BNO055State::instance().reset();
    }
}
