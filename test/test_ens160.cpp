// esp32emu — ENS160 air quality sensor test
#include "ENS160.h"
#include <cassert>
#include <cstdio>

int main() {
    ENS160 sensor;

    // Not initialized
    assert(sensor.getAQI() == 0);
    assert(sensor.getTVOC() == 0);
    assert(sensor.geteCO2() == 0);

    // Init
    assert(sensor.begin(0x53));
    assert(sensor._isInitialized());

    // Default values
    assert(sensor.getAQI() == 1);
    assert(sensor.getTVOC() == 50);
    assert(sensor.geteCO2() == 400);

    // Set mode
    assert(sensor.setMode(ENS160::MODE_STANDARD));

    // Availability
    assert(sensor.available());
    sensor._setDataReady(false);
    assert(!sensor.available());
    sensor._setDataReady(true);

    // Set values
    sensor._setAQI(ENS160::AQI_POOR);
    sensor._setTVOC(500);
    sensor._seteCO2(1200);
    assert(sensor.getAQI() == 4);
    assert(sensor.getTVOC() == 500);
    assert(sensor.geteCO2() == 1200);

    // Compensation
    assert(sensor.setTempCompensation(22.0f));
    assert(sensor.setRHCompensation(45.0f));

    // Hot plate resistance
    sensor._setHP(100, 200, 300, 400);
    assert(sensor.getHP0() == 100);
    assert(sensor.getHP1() == 200);
    assert(sensor.getHP2() == 300);
    assert(sensor.getHP3() == 400);

    // Firmware
    assert(sensor.getFirmwareVersion() == 0x0310);

    // Constants
    assert(ENS160::MODE_DEEP_SLEEP == 0);
    assert(ENS160::MODE_IDLE == 1);
    assert(ENS160::MODE_STANDARD == 2);
    assert(ENS160::AQI_EXCELLENT == 1);
    assert(ENS160::AQI_UNHEALTHY == 5);

    sensor.reset();

    printf("test_ens160: all assertions passed\n");
    return 0;
}
