// Adafruit_LSM9DS1.h — 9-DOF IMU mock for esp32emu
#pragma once
#include "Adafruit_Sensor.h"
#include <cstdint>
#include <cstdio>

typedef enum {
    LSM9DS1_ACCELRANGE_2G  = 0,
    LSM9DS1_ACCELRANGE_4G  = 1,
    LSM9DS1_ACCELRANGE_8G  = 2,
    LSM9DS1_ACCELRANGE_16G = 3
} lsm9ds1AccelRange_t;

typedef enum {
    LSM9DS1_MAGGAIN_4GAUSS  = 0,
    LSM9DS1_MAGGAIN_8GAUSS  = 1,
    LSM9DS1_MAGGAIN_12GAUSS = 2,
    LSM9DS1_MAGGAIN_16GAUSS = 3
} lsm9ds1MagGain_t;

typedef enum {
    LSM9DS1_GYROSCALE_245DPS  = 0,
    LSM9DS1_GYROSCALE_500DPS  = 1,
    LSM9DS1_GYROSCALE_2000DPS = 2
} lsm9ds1GyroScale_t;

typedef enum {
    LSM9DS1_ACCELDATARATE_10HZ  = 1,
    LSM9DS1_ACCELDATARATE_50HZ  = 2,
    LSM9DS1_ACCELDATARATE_119HZ = 3,
    LSM9DS1_ACCELDATARATE_238HZ = 4,
    LSM9DS1_ACCELDATARATE_476HZ = 5,
    LSM9DS1_ACCELDATARATE_952HZ = 6
} lsm9ds1AccelDataRate_t;

typedef enum {
    LSM9DS1_MAGDATARATE_3_125HZ = 0,
    LSM9DS1_MAGDATARATE_6_25HZ  = 1,
    LSM9DS1_MAGDATARATE_12_5HZ  = 2,
    LSM9DS1_MAGDATARATE_25HZ    = 3,
    LSM9DS1_MAGDATARATE_50HZ    = 4,
    LSM9DS1_MAGDATARATE_100HZ   = 5
} lsm9ds1MagDataRate_t;

class Adafruit_LSM9DS1 {
public:
    struct Vector {
        float x = 0, y = 0, z = 0;
    };

    Adafruit_LSM9DS1() = default;

    bool begin() {
        _begun = true;
        printf("[esp32emu] LSM9DS1: 9-DOF IMU initialized\n");
        return true;
    }

    void read() {
        // Simulate reading; values stay at test-injected or default
        printf("[esp32emu] LSM9DS1: read accel=(%.2f,%.2f,%.2f) gyro=(%.2f,%.2f,%.2f) mag=(%.2f,%.2f,%.2f)\n",
               accel.x, accel.y, accel.z, gyro.x, gyro.y, gyro.z, mag.x, mag.y, mag.z);
    }

    bool getEvent(sensors_event_t* accelEvt, sensors_event_t* magEvt,
                  sensors_event_t* gyroEvt, sensors_event_t* tempEvt = nullptr) {
        if (accelEvt) {
            accelEvt->acceleration.x = accel.x;
            accelEvt->acceleration.y = accel.y;
            accelEvt->acceleration.z = accel.z;
        }
        if (magEvt) {
            magEvt->magnetic.x = mag.x;
            magEvt->magnetic.y = mag.y;
            magEvt->magnetic.z = mag.z;
        }
        if (gyroEvt) {
            gyroEvt->gyro.x = gyro.x;
            gyroEvt->gyro.y = gyro.y;
            gyroEvt->gyro.z = gyro.z;
        }
        if (tempEvt) {
            tempEvt->temperature = _temperature;
        }
        return true;
    }

    void setupAccel(lsm9ds1AccelRange_t range) { _accelRange = range; }
    void setupMag(lsm9ds1MagGain_t gain) { _magGain = gain; }
    void setupGyro(lsm9ds1GyroScale_t scale) { _gyroScale = scale; }

    lsm9ds1AccelRange_t getAccelRange() const { return _accelRange; }
    lsm9ds1MagGain_t getMagGain() const { return _magGain; }
    lsm9ds1GyroScale_t getGyroScale() const { return _gyroScale; }

    // Public sensor data (mimics real library)
    Vector accel{0.0f, 0.0f, 9.81f};
    Vector gyro{0.0f, 0.0f, 0.0f};
    Vector mag{25.0f, -10.0f, 45.0f};

    // Test helpers
    void setTestAccel(float x, float y, float z) { accel = {x, y, z}; }
    void setTestGyro(float x, float y, float z) { gyro = {x, y, z}; }
    void setTestMag(float x, float y, float z) { mag = {x, y, z}; }
    void setTestTemperature(float t) { _temperature = t; }

private:
    bool _begun = false;
    float _temperature = 25.0f;
    lsm9ds1AccelRange_t _accelRange = LSM9DS1_ACCELRANGE_2G;
    lsm9ds1MagGain_t _magGain = LSM9DS1_MAGGAIN_4GAUSS;
    lsm9ds1GyroScale_t _gyroScale = LSM9DS1_GYROSCALE_245DPS;
};
