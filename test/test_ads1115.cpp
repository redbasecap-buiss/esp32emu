// esp32emu test — ADS1115 16-bit ADC
#include "ADS1115.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    printf("=== ADS1115 ADC Tests ===\n");

    ADS1115 adc;

    // Init
    assert(!adc.isInitialized());
    assert(adc.begin());
    assert(adc.isInitialized());
    printf("[PASS] Init and begin\n");

    // Default gain
    assert(adc.getGain() == GAIN_TWO);
    printf("[PASS] Default gain is GAIN_TWO\n");

    // Set and read channels
    adc.test_setChannelValue(0, 16384);
    adc.test_setChannelValue(1, 8192);
    adc.test_setChannelValue(2, -4096);
    adc.test_setChannelValue(3, 0);
    assert(adc.readADC_SingleEnded(0) == 16384);
    assert(adc.readADC_SingleEnded(1) == 8192);
    assert(adc.readADC_SingleEnded(2) == -4096);
    assert(adc.readADC_SingleEnded(3) == 0);
    printf("[PASS] Single-ended reads\n");

    // Differential
    assert(adc.readADC_Differential_0_1() == 16384 - 8192);
    assert(adc.readADC_Differential_2_3() == -4096 - 0);
    printf("[PASS] Differential reads\n");

    // Voltage computation with GAIN_TWO (2.048V range)
    float v = adc.computeVolts(16384);
    assert(fabs(v - 1.024f) < 0.001f);  // half scale = half range
    printf("[PASS] computeVolts with GAIN_TWO\n");

    // Change gain and verify
    adc.setGain(GAIN_ONE);
    assert(adc.getGain() == GAIN_ONE);
    v = adc.computeVolts(16384);
    assert(fabs(v - 2.048f) < 0.001f);  // 4.096 * 16384/32767
    printf("[PASS] computeVolts with GAIN_ONE\n");

    // All gain ranges
    adc.setGain(GAIN_TWOTHIRDS);
    v = adc.computeVolts(32767);
    assert(fabs(v - 6.144f) < 0.01f);
    adc.setGain(GAIN_SIXTEEN);
    v = adc.computeVolts(32767);
    assert(fabs(v - 0.256f) < 0.01f);
    printf("[PASS] All gain ranges compute correctly\n");

    // Comparator
    adc.startComparator_SingleEnded(2, 1000);
    assert(adc.getLastConversionResults() == -4096);
    printf("[PASS] Comparator mode\n");

    // Out of range channel
    assert(adc.readADC_SingleEnded(4) == 0);
    printf("[PASS] Out-of-range channel returns 0\n");

    // test_setAllChannels
    adc.test_setAllChannels(100, 200, 300, 400);
    assert(adc.readADC_SingleEnded(0) == 100);
    assert(adc.readADC_SingleEnded(3) == 400);
    printf("[PASS] setAllChannels helper\n");

    // Reset
    adc.reset();
    assert(!adc.isInitialized());
    assert(adc.readADC_SingleEnded(0) == 0);
    assert(adc.getGain() == GAIN_TWO);
    printf("[PASS] Reset clears state\n");

    // Custom address
    ADS1115 adc2(ADS1115_ADDRESS_VDD);
    adc2.begin();
    assert(adc2.isInitialized());
    printf("[PASS] Custom address init\n");

    printf("=== All ADS1115 tests passed! ===\n");
    return 0;
}
