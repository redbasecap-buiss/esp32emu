// esp32emu test — MCP23017 I2C GPIO expander
#include "MCP23017.h"
#include <cassert>
#include <cstdio>

int main() {
    printf("=== MCP23017 GPIO Expander Tests ===\n");

    MCP23017 mcp;

    // Test init
    assert(!mcp.isInitialized());
    assert(mcp.begin(0x20));
    assert(mcp.isInitialized());
    printf("[PASS] Init and begin\n");

    // Default: all inputs (IODIR = 0xFFFF)
    assert(mcp.getIODIR() == 0xFFFF);
    printf("[PASS] Default IODIR is all inputs\n");

    // Set pin 0 as output and write
    mcp.pinMode(0, 0);  // output
    assert((mcp.getIODIR() & 1) == 0);
    mcp.digitalWrite(0, 1);
    assert(mcp.digitalRead(0) == 1);
    mcp.digitalWrite(0, 0);
    assert(mcp.digitalRead(0) == 0);
    printf("[PASS] Single pin output read/write\n");

    // Port A/B writes
    mcp.writeGPIOA(0xAA);
    assert(mcp.readGPIOA() == 0xAA);
    mcp.writeGPIOB(0x55);
    assert(mcp.readGPIOB() == 0x55);
    assert(mcp.readGPIOAB() == 0x55AA);
    printf("[PASS] Port A/B read/write\n");

    // Write all 16 bits
    mcp.writeGPIOAB(0x1234);
    assert(mcp.readGPIOAB() == 0x1234);
    assert(mcp.readGPIOA() == 0x34);
    assert(mcp.readGPIOB() == 0x12);
    printf("[PASS] 16-bit GPIO read/write\n");

    // Pull-ups
    mcp.setPullup(3, true);
    mcp.setPullup(10, true);
    assert(mcp.getPullupAB() == ((1 << 3) | (1 << 10)));
    mcp.setPullup(3, false);
    assert(mcp.getPullupAB() == (1 << 10));
    printf("[PASS] Pull-up configuration\n");

    // Interrupts
    mcp.setInterrupt(5, true);
    mcp.setInterrupt(12, true);
    assert(mcp.getInterruptEnabled() == ((1 << 5) | (1 << 12)));
    printf("[PASS] Interrupt configuration\n");

    // Test helper: inject pin state
    mcp.reset();
    mcp.begin();
    mcp.test_setPin(7, true);
    assert(mcp.digitalRead(7) == 1);
    mcp.test_setPin(7, false);
    assert(mcp.digitalRead(7) == 0);
    printf("[PASS] Test helper pin injection\n");

    // Reset
    mcp.reset();
    assert(!mcp.isInitialized());
    assert(mcp.getIODIR() == 0xFFFF);
    assert(mcp.getGPIO() == 0x0000);
    printf("[PASS] Reset clears state\n");

    // Boundary: pin > 15 ignored
    mcp.begin();
    mcp.digitalWrite(16, 1);
    assert(mcp.readGPIOAB() == 0);
    printf("[PASS] Out-of-range pin ignored\n");

    printf("=== All MCP23017 tests passed! ===\n");
    return 0;
}
