// test_lsm9ds1: LSM9DS1 9-DOF IMU mock tests
#include "Adafruit_LSM9DS1.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_LSM9DS1 imu;
    assert(imu.begin());

    // Default accel has gravity on Z
    assert(fabs(imu.accel.z - 9.81f) < 0.01f);

    // Test injection
    imu.setTestAccel(1.0f, 2.0f, 3.0f);
    imu.setTestGyro(10.0f, 20.0f, 30.0f);
    imu.setTestMag(100.0f, 200.0f, 300.0f);

    assert(fabs(imu.accel.x - 1.0f) < 0.01f);
    assert(fabs(imu.gyro.y - 20.0f) < 0.01f);
    assert(fabs(imu.mag.z - 300.0f) < 0.01f);

    // Read (just prints, doesn't crash)
    imu.read();

    // getEvent
    sensors_event_t accelEvt{}, magEvt{}, gyroEvt{}, tempEvt{};
    imu.setTestTemperature(28.5f);
    assert(imu.getEvent(&accelEvt, &magEvt, &gyroEvt, &tempEvt));
    assert(fabs(accelEvt.acceleration.x - 1.0f) < 0.01f);
    assert(fabs(magEvt.magnetic.y - 200.0f) < 0.01f);
    assert(fabs(gyroEvt.gyro.z - 30.0f) < 0.01f);
    assert(fabs(tempEvt.temperature - 28.5f) < 0.01f);

    // Null event pointers
    assert(imu.getEvent(nullptr, nullptr, nullptr, nullptr));

    // Configuration
    imu.setupAccel(LSM9DS1_ACCELRANGE_16G);
    assert(imu.getAccelRange() == LSM9DS1_ACCELRANGE_16G);
    imu.setupMag(LSM9DS1_MAGGAIN_12GAUSS);
    assert(imu.getMagGain() == LSM9DS1_MAGGAIN_12GAUSS);
    imu.setupGyro(LSM9DS1_GYROSCALE_2000DPS);
    assert(imu.getGyroScale() == LSM9DS1_GYROSCALE_2000DPS);

    printf("test_lsm9ds1: all assertions passed\n");
    return 0;
}
