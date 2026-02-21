#include "MCP4725.h"
#include <cassert>
#include <cmath>

int main() {
    MCP4725 dac;

    assert(!dac.emu_initialized());
    assert(dac.begin(0x62));
    assert(dac.emu_initialized());
    assert(dac.emu_getAddress() == 0x62);

    // Set raw value
    dac.setVoltage(2048, false);
    assert(dac.getValue() == 2048);
    assert(dac.emu_getEEPROM() == 0); // Not written to EEPROM

    // Write to EEPROM
    dac.setVoltage(1000, true);
    assert(dac.getValue() == 1000);
    assert(dac.emu_getEEPROM() == 1000);

    // 12-bit mask
    dac.setVoltage(0xFFFF, false);
    assert(dac.getValue() == 0x0FFF);

    // Float voltage
    dac.setVoltageFloat(1.65f, 3.3f);
    assert(dac.getValue() == 2047 || dac.getValue() == 2048); // ~midpoint
    float v = dac.getVoltageFloat(3.3f);
    assert(std::abs(v - 1.65f) < 0.01f);

    // Power down
    assert(dac.getPowerDown() == MCP4725::PD_NORMAL);
    dac.setPowerDown(MCP4725::PD_100K);
    assert(dac.getPowerDown() == MCP4725::PD_100K);

    printf("test_mcp4725: all assertions passed\n");
    return 0;
}
