#include "Adafruit_LTR390.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    printf("test_ltr390: ");

    Adafruit_LTR390 ltr;
    assert(ltr.begin());

    // Enable and mode
    ltr.enable(true);
    assert(ltr.enabled());
    assert(ltr.newDataAvailable());

    ltr.setMode(LTR390_MODE_UVS);
    assert(ltr.getMode() == LTR390_MODE_UVS);

    ltr.setMode(LTR390_MODE_ALS);
    assert(ltr.getMode() == LTR390_MODE_ALS);

    // Gain and resolution
    ltr.setGain(LTR390_GAIN_18);
    assert(ltr.getGain() == LTR390_GAIN_18);
    ltr.setResolution(LTR390_RESOLUTION_20BIT);
    assert(ltr.getResolution() == LTR390_RESOLUTION_20BIT);

    // Read defaults
    assert(ltr.readALS() == 500);
    assert(ltr.readUVS() == 120);

    // Test helpers
    ltr._setALS(1000);
    ltr._setUVS(250);
    assert(ltr.readALS() == 1000);
    assert(ltr.readUVS() == 250);

    // Lux calculation (should be > 0)
    ltr.setGain(LTR390_GAIN_3);
    ltr.setResolution(LTR390_RESOLUTION_18BIT);
    ltr._setALS(500);
    float lux = ltr.getLux();
    assert(lux > 0);

    // UVI calculation
    ltr._setUVS(120);
    float uvi = ltr.getUVI();
    assert(uvi > 0);

    // Thresholds and interrupts (just verify they don't crash)
    ltr.setThresholds(100, 5000);
    ltr.configInterrupt(true, LTR390_MODE_ALS, 4);

    // Disable
    ltr.enable(false);
    assert(!ltr.enabled());

    printf("all assertions passed\n");
    return 0;
}
