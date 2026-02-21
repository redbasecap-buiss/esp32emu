#include "Adafruit_MAX31865.h"
#include <cassert>
#include <cmath>

int main() {
    Adafruit_MAX31865 sensor(5); // CS pin 5

    // Init
    assert(!sensor.emu_initialized());
    assert(sensor.begin(Adafruit_MAX31865::MAX31865_3WIRE));
    assert(sensor.emu_initialized());

    // RTD reading
    sensor.emu_setRTD(8192);
    assert(sensor.readRTD() == 8192);

    // Temperature via mock (returns injected value)
    sensor.emu_setTemperature(25.0f);
    float t = sensor.temperature(100.0f, 430.0f);
    assert(std::abs(t - 25.0f) < 0.1f);

    sensor.emu_setTemperature(-10.5f);
    assert(std::abs(sensor.temperature() - (-10.5f)) < 0.1f);

    // Fault detection
    assert(sensor.readFault() == 0);
    sensor.emu_setFault(Adafruit_MAX31865::MAX31865_FAULT_HIGHTHRESH);
    assert(sensor.readFault() == 0x80);
    sensor.clearFault();
    assert(sensor.readFault() == 0);

    // Bias and auto-convert
    sensor.enableBias(true);
    assert(sensor.emu_biasEnabled());
    sensor.autoConvert(true);
    sensor.enable50Hz(true);
    sensor.setThresholds(100, 30000);

    printf("test_max31865: all assertions passed\n");
    return 0;
}
