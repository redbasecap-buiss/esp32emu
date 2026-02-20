// MCP23017 GPIO Expander — button input and LED output
// Reads buttons on port A, drives LEDs on port B

#include <Arduino.h>
#include <Wire.h>
#include <MCP23017.h>

MCP23017 mcp;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    mcp.begin(0x20);

    // Port A: inputs with pull-ups (buttons)
    for (int i = 0; i < 8; i++) {
        mcp.pinMode(i, 1);       // input
        mcp.setPullup(i, true);  // pull-up
    }
    // Port B: outputs (LEDs)
    for (int i = 8; i < 16; i++) {
        mcp.pinMode(i, 0);  // output
    }

    Serial.println("MCP23017 GPIO Expander ready");
}

void loop() {
    uint8_t buttons = mcp.readGPIOA();
    // Mirror button state to LEDs (active low buttons, active high LEDs)
    mcp.writeGPIOB(~buttons);

    Serial.print("Buttons: 0b");
    for (int i = 7; i >= 0; i--) {
        Serial.print((buttons >> i) & 1);
    }
    Serial.print("  LEDs: 0b");
    uint8_t leds = mcp.readGPIOB();
    for (int i = 7; i >= 0; i--) {
        Serial.print((leds >> i) & 1);
    }
    Serial.println();
    delay(200);
}
