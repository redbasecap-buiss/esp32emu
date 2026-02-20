#include "VL53L0X.h"
#include <cassert>
#include <cstdio>

int main() {
    VL53L0X sensor;
    assert(!sensor.isBegun());
    assert(sensor.begin());
    assert(sensor.isBegun());
    assert(sensor.getAddress() == 0x29);

    // Default distance
    assert(sensor.readRangeSingleMillimeters() == 100);

    // Set distance
    sensor.setDistance(350);
    assert(sensor.readRangeSingleMillimeters() == 350);
    assert(sensor.readRangeContinuousMillimeters() == 350);
    assert(sensor.read() == 350);

    // Continuous mode
    assert(!sensor.isContinuous());
    sensor.startContinuous(50);
    assert(sensor.isContinuous());
    assert(sensor.dataReady());
    sensor.stopContinuous();
    assert(!sensor.isContinuous());

    // Timeout
    sensor.setTimeout(1000);
    assert(sensor.getTimeout() == 1000);
    assert(!sensor.timeoutOccurred());

    // Timing budget
    assert(sensor.setMeasurementTimingBudget(33000));
    assert(sensor.getMeasurementTimingBudget() == 33000);
    assert(!sensor.setMeasurementTimingBudget(10000)); // too small

    // Address change
    sensor.setAddress(0x30);
    assert(sensor.getAddress() == 0x30);

    // Signal quality
    sensor.setSignalRate(8000);
    assert(sensor.getSignalRateReturn() == 8000);
    sensor.setAmbientRate(100);
    assert(sensor.getAmbientRateReturn() == 100);
    assert(sensor.getSpadCount() == 8);

    // Range status
    assert(sensor.getRangeStatus() == VL53L0X::RANGE_VALID);

    // Long range config
    assert(sensor.setSignalRateLimit(0.1f));

    printf("test_vl53l0x: all assertions passed\n");
    return 0;
}
