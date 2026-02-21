#include "Adafruit_VEML6075.h"
#include <cassert>
#include <cmath>

int main() {
    Adafruit_VEML6075 sensor;

    assert(!sensor.emu_initialized());
    assert(sensor.begin());
    assert(sensor.emu_initialized());

    // Default UV readings
    assert(std::abs(sensor.readUVA() - 15.0f) < 0.1f);
    assert(std::abs(sensor.readUVB() - 8.0f) < 0.1f);
    assert(std::abs(sensor.readUVI() - 3.5f) < 0.1f);

    // Inject values
    sensor.emu_setUVA(25.0f);
    sensor.emu_setUVB(12.0f);
    sensor.emu_setUVI(6.0f);
    assert(std::abs(sensor.readUVA() - 25.0f) < 0.1f);
    assert(std::abs(sensor.readUVB() - 12.0f) < 0.1f);
    assert(std::abs(sensor.readUVI() - 6.0f) < 0.1f);

    // Settings
    sensor.setIntegrationTime(Adafruit_VEML6075::VEML6075_200MS);
    assert(sensor.getIntegrationTime() == Adafruit_VEML6075::VEML6075_200MS);
    sensor.setHighDynamic(true);
    sensor.setForcedMode(true);
    sensor.setCoefficients(2.0f, 1.0f, 3.0f, 2.0f, 0.001f, 0.002f);

    printf("test_veml6075: all assertions passed\n");
    return 0;
}
