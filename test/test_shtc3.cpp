// esp32emu test — SHTC3 humidity/temperature sensor
#include "SHTC3.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static void test_init_and_read() {
    esp32emu_shtc3_test::reset();
    SHTC3 sensor;
    assert(sensor.begin() == SHTC3_Status_Nominal);
    assert(sensor.update() == SHTC3_Status_Nominal);
    assert(fabs(sensor.toDegC() - 22.5f) < 0.1f);
    assert(fabs(sensor.toPercent() - 45.0f) < 0.1f);
}

static void test_inject_values() {
    esp32emu_shtc3_test::reset();
    SHTC3 sensor;
    sensor.begin();
    esp32emu_shtc3_test::setTemperature(35.2f);
    esp32emu_shtc3_test::setHumidity(80.0f);
    sensor.update();
    assert(fabs(sensor.toDegC() - 35.2f) < 0.1f);
    assert(fabs(sensor.toPercent() - 80.0f) < 0.1f);
}

static void test_fahrenheit() {
    esp32emu_shtc3_test::reset();
    SHTC3 sensor;
    sensor.begin();
    esp32emu_shtc3_test::setTemperature(0.0f);
    sensor.update();
    assert(fabs(sensor.toDegF() - 32.0f) < 0.1f);

    esp32emu_shtc3_test::setTemperature(100.0f);
    sensor.update();
    assert(fabs(sensor.toDegF() - 212.0f) < 0.1f);
}

static void test_sleep_wake() {
    esp32emu_shtc3_test::reset();
    SHTC3 sensor;
    sensor.begin();
    assert(sensor.sleep() == SHTC3_Status_Nominal);
    assert(sensor.update() == SHTC3_Status_Error);
    assert(sensor.wake() == SHTC3_Status_Nominal);
    assert(sensor.update() == SHTC3_Status_Nominal);
}

static void test_raw_data() {
    esp32emu_shtc3_test::reset();
    SHTC3 sensor;
    sensor.begin();
    esp32emu_shtc3_test::setTemperature(25.0f);
    esp32emu_shtc3_test::setHumidity(60.0f);
    sensor.update();
    auto raw = sensor.toRawData();
    assert(fabs(raw.toDegC() - 25.0f) < 0.1f);
    assert(fabs(raw.toPercent() - 60.0f) < 0.1f);
    assert(raw.status == SHTC3_Status_Nominal);
}

int main() {
    test_init_and_read();
    test_inject_values();
    test_fahrenheit();
    test_sleep_wake();
    test_raw_data();
    printf("test_shtc3: all assertions passed\n");
    return 0;
}
