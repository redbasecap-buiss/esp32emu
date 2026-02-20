#include "MAX31855.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_MAX31855 tc(13, 10, 12);  // CLK, CS, MISO
    assert(!tc.isBegun());
    assert(tc.begin());
    assert(tc.isBegun());

    // Default temperature
    assert(fabs(tc.readCelsius() - 25.0) < 0.1);
    assert(fabs(tc.readFahrenheit() - 77.0) < 0.1);
    assert(fabs(tc.readInternal() - 25.0) < 0.1);

    // Set temperature
    tc.setTemperature(100.0);
    assert(fabs(tc.readCelsius() - 100.0) < 0.1);
    assert(fabs(tc.readFahrenheit() - 212.0) < 0.1);

    // Negative temperature
    tc.setTemperature(-40.0);
    assert(fabs(tc.readCelsius() - (-40.0)) < 0.1);
    assert(fabs(tc.readFahrenheit() - (-40.0)) < 0.1);  // -40 == -40 in F

    // Internal temperature
    tc.setInternalTemperature(30.0);
    assert(fabs(tc.readInternal() - 30.0) < 0.1);

    // No fault initially
    assert(tc.readError() == 0);

    // Open circuit fault
    tc.setFault(Adafruit_MAX31855::FAULT_OPEN);
    assert(tc.readError() == Adafruit_MAX31855::FAULT_OPEN);
    assert(std::isnan(tc.readCelsius()));
    assert(std::isnan(tc.readFahrenheit()));

    // Short to GND
    tc.setFault(Adafruit_MAX31855::FAULT_SHORT_GND);
    assert(tc.readError() == Adafruit_MAX31855::FAULT_SHORT_GND);

    // Clear fault
    tc.clearFault();
    assert(tc.readError() == 0);
    tc.setTemperature(25.0);
    assert(fabs(tc.readCelsius() - 25.0) < 0.1);

    // Raw32
    uint32_t raw = tc.readRaw32();
    assert(raw != 0);

    // SPI constructor
    Adafruit_MAX31855 tc2(10);
    tc2.begin();
    assert(tc2.isBegun());

    printf("test_max31855: all assertions passed\n");
    return 0;
}
