#include "MPU6050.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    MPU6050 mpu;
    assert(mpu.getAddress() == 0x68);
    assert(!mpu.isBegun());

    mpu.begin();
    assert(mpu.isBegun());

    // Default: 1g on Z
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    assert(ax == 0);
    assert(ay == 0);
    assert(az == 16384);
    assert(gx == 0 && gy == 0 && gz == 0);

    // G conversion
    assert(fabs(mpu.getAccelG_Z() - 1.0f) < 0.01f);
    assert(fabs(mpu.getAccelG_X()) < 0.01f);

    // Set acceleration (tilt 45 degrees)
    mpu.setAcceleration(11585, 0, 11585);  // ~0.707g on X and Z
    float pitch = mpu.getPitch();
    assert(fabs(pitch - 45.0f) < 1.0f);

    // Gyro
    mpu.setRotation(131, 0, 0);  // 1 deg/s on X
    assert(fabs(mpu.getGyroDPS_X() - 1.0f) < 0.01f);

    // Temperature
    assert(mpu.getTemperature() == 25.0f);
    mpu.setTemperature(36.5f);
    assert(mpu.getTemperature() == 36.5f);

    // Individual accessors
    mpu.setAcceleration(100, 200, 300);
    assert(mpu.getAccelerationX() == 100);
    assert(mpu.getAccelerationY() == 200);
    assert(mpu.getAccelerationZ() == 300);

    // Custom address
    MPU6050 mpu2(0x69);
    assert(mpu2.getAddress() == 0x69);

    printf("test_mpu6050: all assertions passed\n");
    return 0;
}
