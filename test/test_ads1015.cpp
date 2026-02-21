#include "ADS1015.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_ADS1015 adc;
    assert(adc.begin(0x48));

    // Test gain
    adc.setGain(Adafruit_ADS1015::GAIN_ONE);
    assert(adc.getGain() == Adafruit_ADS1015::GAIN_ONE);

    // Test single-ended read
    adc.test_setValue(0, 1000);
    adc.test_setValue(1, 500);
    adc.test_setValue(2, 200);
    adc.test_setValue(3, 100);
    assert(adc.readADC_SingleEnded(0) == 1000);
    assert(adc.readADC_SingleEnded(1) == 500);
    assert(adc.readADC_SingleEnded(3) == 100);

    // Test differential
    assert(adc.readADC_Differential_0_1() == 500);
    assert(adc.readADC_Differential_2_3() == 100);

    // Test voltage computation (GAIN_ONE = 4.096V range)
    float v = adc.computeVolts(1024);
    assert(fabs(v - 2.048f) < 0.01f);

    // Test out-of-range channel
    assert(adc.readADC_SingleEnded(5) == 0);

    printf("test_ads1015: all assertions passed\n");
    return 0;
}
