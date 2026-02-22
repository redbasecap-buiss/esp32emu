// MCP2515 CAN Bus — Send and Receive
// Demonstrates CAN communication with SPI-based MCP2515 controller.

#include <Arduino.h>
#include <SPI.h>
#include <MCP2515.h>

#define CAN_CS_PIN 5

MCP2515 can(CAN_CS_PIN);

void setup() {
    Serial.begin(115200);
    SPI.begin();

    can.reset();
    can.setBitrate(MCP2515_NS::CAN_500KBPS, MCP2515_NS::MCP_8MHZ);
    can.setLoopbackMode();  // Loopback for testing

    Serial.println("MCP2515 CAN Bus Demo (Loopback Mode)");

    // Send a frame
    can_frame txMsg = {};
    txMsg.can_id = 0x100;
    txMsg.can_dlc = 4;
    txMsg.data[0] = 0xCA;
    txMsg.data[1] = 0xFE;
    txMsg.data[2] = 0xBA;
    txMsg.data[3] = 0xBE;

    if (can.sendMessage(&txMsg) == MCP2515_NS::ERROR_OK) {
        Serial.print("📤 Sent CAN ID: 0x");
        Serial.println(txMsg.can_id, HEX);
    }
}

void loop() {
    can_frame rxMsg = {};
    if (can.readMessage(&rxMsg) == MCP2515_NS::ERROR_OK) {
        Serial.print("📥 Received CAN ID: 0x");
        Serial.print(rxMsg.can_id, HEX);
        Serial.print(" Data: ");
        for (int i = 0; i < rxMsg.can_dlc; i++) {
            if (rxMsg.data[i] < 0x10) Serial.print("0");
            Serial.print(rxMsg.data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    delay(100);
}
