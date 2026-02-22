// esp32emu — test_sgp40: SGP40 VOC sensor mock test
#include "Adafruit_SGP40.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_SGP40 sgp;

    assert(!sgp.test_isBegun());
    assert(sgp.begin());
    assert(sgp.test_isBegun());

    // Default readings
    assert(sgp.measureRaw() == 26000);
    assert(sgp.measureVocIndex() == 100);

    // Self test
    assert(sgp.selfTest());

    // Set custom readings
    sgp.test_setRawVOC(30000);
    sgp.test_setVocIndex(250);
    assert(sgp.measureRaw() == 30000);
    assert(sgp.measureVocIndex() == 250);
    assert(sgp.test_getRawVOC() == 30000);
    assert(sgp.test_getVocIndex() == 250);

    // Temperature/humidity compensation (accepted but mock ignores)
    uint16_t raw = sgp.measureRaw(28.5f, 60.0f);
    assert(raw == 30000);

    // Heater off
    sgp.heaterOff();

    printf("test_sgp40: all assertions passed\n");
    return 0;
}
