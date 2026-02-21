#include "Adafruit_VEML7700.h"
#include <cassert>
#include <cmath>

int main() {
    Adafruit_VEML7700 sensor;

    // Init
    assert(!sensor.emu_initialized());
    assert(sensor.begin());
    assert(sensor.emu_initialized());

    // Default readings
    assert(std::abs(sensor.readLux() - 250.0f) < 0.1f);
    assert(sensor.readALS() == 1000);
    assert(sensor.readWhite() == 1100);

    // Inject test values
    sensor.emu_setLux(500.0f);
    sensor.emu_setALS(2000);
    sensor.emu_setWhite(2200);
    assert(std::abs(sensor.readLux() - 500.0f) < 0.1f);
    assert(sensor.readALS() == 2000);
    assert(sensor.readWhite() == 2200);

    // Gain and integration time
    sensor.setGain(Adafruit_VEML7700::VEML7700_GAIN_2);
    assert(sensor.getGain() == Adafruit_VEML7700::VEML7700_GAIN_2);
    sensor.setIntegrationTime(Adafruit_VEML7700::VEML7700_IT_200MS);
    assert(sensor.getIntegrationTime() == Adafruit_VEML7700::VEML7700_IT_200MS);

    // Interrupts and power save
    sensor.interruptEnable(true);
    sensor.setLowThreshold(100);
    sensor.setHighThreshold(5000);
    assert(sensor.interruptStatus() == 0);
    sensor.powerSaveEnable(true);
    sensor.setPowerSaveMode(2);

    printf("test_veml7700: all assertions passed\n");
    return 0;
}
