#include "SparkFunBMI270.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    BMI270 imu;

    // Test init
    assert(imu.beginI2C(0x68) == 0);

    // Test default accel (Z = gravity)
    assert(imu.getSensorData() == 0);
    assert(fabs(imu.accX()) < 0.01f);
    assert(fabs(imu.accY()) < 0.01f);
    assert(fabs(imu.accZ() - 9.81f) < 0.01f);

    // Test gyro defaults (zero)
    assert(fabs(imu.gyrX()) < 0.01f);
    assert(fabs(imu.gyrY()) < 0.01f);
    assert(fabs(imu.gyrZ()) < 0.01f);

    // Test injection
    imu.test_setAccel(1.0f, 2.0f, 3.0f);
    assert(fabs(imu.accX() - 1.0f) < 0.01f);
    assert(fabs(imu.accY() - 2.0f) < 0.01f);
    assert(fabs(imu.accZ() - 3.0f) < 0.01f);

    imu.test_setGyro(10.0f, 20.0f, 30.0f);
    assert(fabs(imu.gyrX() - 10.0f) < 0.01f);
    assert(fabs(imu.gyrY() - 20.0f) < 0.01f);

    // Test temperature
    assert(fabs(imu.temperature() - 25.0f) < 0.01f);
    imu.test_setTemp(30.5f);
    assert(fabs(imu.temperature() - 30.5f) < 0.01f);

    // Test step counter
    assert(imu.enableStepCounter() == 0);
    imu.test_setSteps(1234);
    assert(imu.getStepCount() == 1234);

    // Test config methods return OK
    assert(imu.setAccelODR(8) == 0);
    assert(imu.setAccelRange(2) == 0);
    assert(imu.setGyroODR(8) == 0);
    assert(imu.setGyroRange(1) == 0);

    printf("test_bmi270: all assertions passed\n");
    return 0;
}
