// esp32emu test — Adafruit_BNO055 9-DOF IMU
#include "Adafruit_BNO055.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static void test_init_and_mode() {
    esp32emu_bno055_test::reset();
    Adafruit_BNO055 bno;
    assert(bno.begin(OPERATION_MODE_NDOF));
    assert(bno.getMode() == OPERATION_MODE_NDOF);
    bno.setMode(OPERATION_MODE_IMUPLUS);
    assert(bno.getMode() == OPERATION_MODE_IMUPLUS);
}

static void test_euler_angles() {
    esp32emu_bno055_test::reset();
    Adafruit_BNO055 bno;
    bno.begin();
    esp32emu_bno055_test::setEuler(180.0, 15.5, -3.2);
    auto e = bno.getVector(VECTOR_EULER);
    assert(fabs(e.x() - 180.0) < 0.01);
    assert(fabs(e.y() - 15.5) < 0.01);
    assert(fabs(e.z() - (-3.2)) < 0.01);
}

static void test_quaternion() {
    esp32emu_bno055_test::reset();
    Adafruit_BNO055 bno;
    bno.begin();
    esp32emu_bno055_test::setQuat(0.707, 0.0, 0.707, 0.0);
    auto q = bno.getQuat();
    assert(fabs(q.w() - 0.707) < 0.001);
    assert(fabs(q.y() - 0.707) < 0.001);
}

static void test_accel_gyro_mag() {
    esp32emu_bno055_test::reset();
    Adafruit_BNO055 bno;
    bno.begin();
    esp32emu_bno055_test::setAccel(1.0, -2.0, 9.81);
    esp32emu_bno055_test::setGyro(0.1, -0.2, 0.3);
    esp32emu_bno055_test::setMag(25.0, -10.0, 50.0);

    auto a = bno.getVector(VECTOR_ACCELEROMETER);
    assert(fabs(a.x() - 1.0) < 0.01);
    assert(fabs(a.z() - 9.81) < 0.01);

    auto g = bno.getVector(VECTOR_GYROSCOPE);
    assert(fabs(g.y() - (-0.2)) < 0.01);

    auto m = bno.getVector(VECTOR_MAGNETOMETER);
    assert(fabs(m.z() - 50.0) < 0.01);
}

static void test_linear_accel_gravity() {
    esp32emu_bno055_test::reset();
    Adafruit_BNO055 bno;
    bno.begin();
    esp32emu_bno055_test::setLinearAccel(0.5, -0.3, 0.1);
    esp32emu_bno055_test::setGravity(0.0, 0.0, 9.81);

    auto la = bno.getVector(VECTOR_LINEARACCEL);
    assert(fabs(la.x() - 0.5) < 0.01);

    auto gv = bno.getVector(VECTOR_GRAVITY);
    assert(fabs(gv.z() - 9.81) < 0.01);
}

static void test_temperature() {
    esp32emu_bno055_test::reset();
    Adafruit_BNO055 bno;
    bno.begin();
    esp32emu_bno055_test::setTemp(30);
    assert(bno.getTemp() == 30);
}

static void test_calibration() {
    esp32emu_bno055_test::reset();
    Adafruit_BNO055 bno;
    bno.begin();
    assert(bno.isFullyCalibrated());

    esp32emu_bno055_test::setCalibration(2, 3, 1, 3);
    assert(!bno.isFullyCalibrated());

    uint8_t s, g, a, m;
    bno.getCalibration(&s, &g, &a, &m);
    assert(s == 2 && g == 3 && a == 1 && m == 3);
}

static void test_offsets() {
    Adafruit_BNO055 bno;
    bno.begin();
    uint8_t buf[22];
    assert(bno.getSensorOffsets(buf));
    bno.setSensorOffsets(buf); // should not crash
}

int main() {
    test_init_and_mode();
    test_euler_angles();
    test_quaternion();
    test_accel_gyro_mag();
    test_linear_accel_gravity();
    test_temperature();
    test_calibration();
    test_offsets();
    printf("test_bno055: all assertions passed\n");
    return 0;
}
