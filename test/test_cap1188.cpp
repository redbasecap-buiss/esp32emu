// esp32emu — CAP1188 capacitive touch sensor test
#include "CAP1188.h"
#include <cassert>
#include <cstdio>

int main() {
    CAP1188 sensor;

    // Not initialized
    assert(sensor.touched() == 0);
    assert(!sensor.isTouched(0));

    // Init
    assert(sensor.begin(0x29));
    assert(sensor._isInitialized());

    // No touches initially
    assert(sensor.touched() == 0);

    // Set single touch
    sensor._setChannelTouched(3, true);
    assert(sensor.isTouched(3));
    assert(!sensor.isTouched(0));

    // touched() reads and clears
    sensor._setTouchState(0b00001010);  // channels 1 and 3
    uint8_t t = sensor.touched();
    assert(t == 0b00001010);
    assert(sensor.touched() == 0);  // cleared after read

    // Multi-touch
    sensor.enableMultiTouch(true, 4);

    // Sensitivity
    sensor.setSensitivity(7);
    assert(sensor.getSensitivity() == 7);

    // Thresholds
    sensor.setThreshold(0, 32);
    sensor.setThreshold(7, 128);
    sensor.setThreshold(8, 99);  // out of range, no crash

    // IDs
    assert(sensor.getProductID() == 0x50);
    assert(sensor.getManufacturerID() == 0x5D);
    assert(sensor.getRevision() == 0x83);

    // Register access
    sensor.readRegister(0x00);
    sensor.writeRegister(0x00, 0xFF);
    sensor.resetInterrupt();

    // Invalid channel
    assert(!sensor.isTouched(8));

    printf("test_cap1188: all assertions passed\n");
    return 0;
}
