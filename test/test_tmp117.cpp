// esp32emu — TMP117 precision temperature sensor test
#include "TMP117.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    TMP117 sensor;

    // Not initialized
    assert(std::isnan(sensor.readTemperature()));
    assert(!sensor.dataReady());

    // Init
    assert(sensor.begin(0x48));
    assert(sensor._isInitialized());
    assert(sensor._getAddr() == 0x48);

    // Default reading
    float t = sensor.readTemperature();
    assert(t > 22.0f && t < 23.0f);

    // Set temperature
    sensor._setTemperature(36.6f);
    assert(sensor.dataReady());
    t = sensor.readTempC();
    assert(t > 36.5f && t < 36.7f);

    // Fahrenheit
    sensor._setTemperature(100.0f);
    float f = sensor.readTempF();
    assert(f > 211.9f && f < 212.1f);

    // Alerts
    sensor.setHighLimit(50.0f);
    sensor.setLowLimit(10.0f);
    sensor._setTemperature(60.0f);
    assert(sensor.getHighAlert());
    assert(!sensor.getLowAlert());

    sensor._setTemperature(5.0f);
    assert(!sensor.getHighAlert());
    assert(sensor.getLowAlert());

    sensor._setTemperature(25.0f);
    assert(!sensor.getHighAlert());
    assert(!sensor.getLowAlert());

    // Limits
    assert(sensor.getHighLimit() == 50.0f);
    assert(sensor.getLowLimit() == 10.0f);

    // Device ID
    assert(sensor.getDeviceID() == 0x0117);

    // Config
    sensor.setConversionMode(2);
    sensor.setConversionCycleTime(4);
    sensor.setAveraging(3);
    sensor.softReset();

    printf("test_tmp117: all assertions passed\n");
    return 0;
}
