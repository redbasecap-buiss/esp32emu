// test_tcs34725: TCS34725 color sensor mock tests
#include "Adafruit_TCS34725.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_TCS34725 sensor(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
    assert(sensor.begin());

    // Config
    assert(sensor.getIntegrationTime() == TCS34725_INTEGRATIONTIME_50MS);
    assert(sensor.getGain() == TCS34725_GAIN_4X);
    sensor.setIntegrationTime(TCS34725_INTEGRATIONTIME_700MS);
    assert(sensor.getIntegrationTime() == TCS34725_INTEGRATIONTIME_700MS);
    sensor.setGain(TCS34725_GAIN_60X);
    assert(sensor.getGain() == TCS34725_GAIN_60X);

    // Default raw data
    uint16_t r, g, b, c;
    sensor.getRawData(&r, &g, &b, &c);
    assert(r == 200 && g == 180 && b == 150 && c == 530);

    // Test injection
    sensor.setTestColor(1000, 800, 600, 2400);
    sensor.getRawData(&r, &g, &b, &c);
    assert(r == 1000 && g == 800 && b == 600 && c == 2400);

    // One-shot
    uint16_t r2, g2, b2, c2;
    assert(sensor.getRawDataOneShot(&r2, &g2, &b2, &c2));
    assert(r2 == 1000);

    // Lux calculation
    uint16_t lux = sensor.calculateLux(r, g, b);
    assert(lux > 0);

    // Color temperature
    uint16_t cct = sensor.calculateColorTemperature(r, g, b);
    assert(cct > 0);
    uint16_t cct2 = sensor.calculateColorTemperature_dn40(r, g, b, c);
    assert(cct2 > 0);

    // Enable/disable
    assert(sensor.isEnabled());
    sensor.disable();
    assert(!sensor.isEnabled());
    sensor.enable();
    assert(sensor.isEnabled());

    // Interrupt
    sensor.setInterrupt(true);
    sensor.clearInterrupt();
    sensor.setIntLimits(100, 1000);

    printf("test_tcs34725: all assertions passed\n");
    return 0;
}
