// esp32emu test — PCF8574 I2C I/O Expander
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "PCF8574.h"

int main() {
    // Test construction and init
    {
        PCF8574 expander(0x20);
        assert(!expander.isConnected());
        assert(expander.begin());
        assert(expander.isConnected());
        assert(expander.getAddress() == 0x20);
    }

    // Test write8/read8
    {
        PCF8574 expander(0x21);
        expander.begin();
        assert(expander.write8(0xAB));
        assert(expander.read8() == 0xAB);
        assert(expander.write8(0x00));
        assert(expander.read8() == 0x00);
    }

    // Test single pin write/read
    {
        PCF8574 expander;
        expander.begin();
        expander.write8(0x00);
        assert(expander.write(3, HIGH));
        assert(expander.read(3) == 1);
        assert(expander.read(0) == 0);
        assert(expander.write(3, LOW));
        assert(expander.read(3) == 0);
    }

    // Test invalid pin
    {
        PCF8574 expander;
        expander.begin();
        assert(!expander.write(8, HIGH));
        assert(expander.read(8) == 0);
        assert(!expander.toggle(8));
    }

    // Test toggle
    {
        PCF8574 expander;
        expander.begin();
        expander.write8(0x00);
        assert(expander.toggle(0));
        assert(expander.read(0) == 1);
        assert(expander.toggle(0));
        assert(expander.read(0) == 0);
    }

    // Test helpers
    {
        PCF8574 expander;
        expander.begin();
        expander.test_setInput(0b10101010);
        assert(expander.read8() == 0xAA);
        assert(expander.read(1) == 1);
        assert(expander.read(0) == 0);
        expander.test_setPin(0, true);
        assert(expander.read(0) == 1);
        assert(expander.test_getOutput() == 0xAB);
    }

    // Test setPolarity8 / setButtonMask
    {
        PCF8574 expander;
        expander.begin();
        assert(expander.setPolarity8(0xFF));
        expander.setButtonMask(0x0F);
    }

    printf("test_pcf8574: all assertions passed\n");
    return 0;
}
