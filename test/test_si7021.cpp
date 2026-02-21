#include "Adafruit_SI7021.h"
#include <cassert>
#include <cmath>

int main() {
    Adafruit_SI7021 sensor;

    assert(!sensor.emu_initialized());
    assert(sensor.begin());
    assert(sensor.emu_initialized());

    // Default readings
    assert(std::abs(sensor.readTemperature() - 22.5f) < 0.1f);
    assert(std::abs(sensor.readHumidity() - 45.0f) < 0.1f);

    // Inject values
    sensor.emu_setTemperature(30.0f);
    sensor.emu_setHumidity(65.0f);
    assert(std::abs(sensor.readTemperature() - 30.0f) < 0.1f);
    assert(std::abs(sensor.readHumidity() - 65.0f) < 0.1f);

    // Resolution
    sensor.setResolution(2);
    assert(sensor.getResolution() == 2);

    // Heater
    assert(!sensor.isHeaterEnabled());
    sensor.heater(true);
    assert(sensor.isHeaterEnabled());

    // Revision
    assert(sensor.getRevision() == 0x20);

    printf("test_si7021: all assertions passed\n");
    return 0;
}
