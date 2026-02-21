#include "MAX30105.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    MAX30105 sensor;

    // Test begin
    assert(sensor.begin());
    assert(sensor.available());

    // Test setup
    sensor.setup(0x1F, 4, 2, 100, 411, 4096);

    // Test default values
    assert(sensor.getIR() == 70000);
    assert(sensor.getRed() == 65000);
    assert(sensor.getGreen() == 0);

    // Test injection
    sensor.test_setIR(80000);
    sensor.test_setRed(75000);
    sensor.test_setGreen(5000);
    assert(sensor.getIR() == 80000);
    assert(sensor.getRed() == 75000);
    assert(sensor.getGreen() == 5000);

    // Test temperature
    float t = sensor.readTemperature();
    assert(fabs(t - 25.0f) < 0.1f);
    sensor.test_setTemperature(36.5f);
    assert(fabs(sensor.readTemperature() - 36.5f) < 0.1f);
    float tf = sensor.readTemperatureF();
    assert(fabs(tf - 97.7f) < 0.2f);

    // Test part ID
    assert(sensor.getPartID() == 0x15);
    assert(sensor.getRevisionID() == 0x03);

    // Test beat detection
    assert(spo2_algorithm::checkForBeat(70000) == true);
    assert(spo2_algorithm::checkForBeat(30000) == false);

    // Test shutdown/wakeup
    sensor.shutDown();
    sensor.wakeUp();
    sensor.softReset();

    printf("test_max30105: all assertions passed\n");
    return 0;
}
