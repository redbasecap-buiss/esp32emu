// esp32emu test — ModbusMaster mock
#include "Arduino.h"
#include "ModbusMaster.h"
#include <cassert>
#include <cstdio>

int main() {
    // Basic init
    ModbusMaster node;
    node.begin(1, Serial2);
    assert(node.emu_getSlaveId() == 1);

    // Write and read holding registers
    node.emu_setRegister(0x0000, 1234);
    node.emu_setRegister(0x0001, 5678);
    uint8_t result = node.readHoldingRegisters(0x0000, 2);
    assert(result == ModbusMaster::ku8MBSuccess);
    assert(node.getResponseBuffer(0) == 1234);
    assert(node.getResponseBuffer(1) == 5678);

    // Write single register
    result = node.writeSingleRegister(0x0010, 42);
    assert(result == ModbusMaster::ku8MBSuccess);
    assert(node.emu_getRegister(0x0010) == 42);

    // Write multiple registers
    node.clearTransmitBuffer();
    node.setTransmitBuffer(0, 100);
    node.setTransmitBuffer(1, 200);
    node.setTransmitBuffer(2, 300);
    result = node.writeMultipleRegisters(0x0020, 3);
    assert(result == ModbusMaster::ku8MBSuccess);
    assert(node.emu_getRegister(0x0020) == 100);
    assert(node.emu_getRegister(0x0021) == 200);
    assert(node.emu_getRegister(0x0022) == 300);

    // Coils
    result = node.writeSingleCoil(0x0000, 1);
    assert(result == ModbusMaster::ku8MBSuccess);
    assert(node.emu_getCoil(0x0000) == 1);

    result = node.readCoils(0x0000, 1);
    assert(result == ModbusMaster::ku8MBSuccess);

    // Read input registers
    node.emu_setRegister(0x0100, 9999);
    result = node.readInputRegisters(0x0100, 1);
    assert(result == ModbusMaster::ku8MBSuccess);
    assert(node.getResponseBuffer(0) == 9999);

    // Failure simulation
    node.emu_setFailNext(true);
    result = node.readHoldingRegisters(0x0000, 1);
    assert(result == ModbusMaster::ku8MBResponseTimedOut);

    // Next call succeeds again
    result = node.readHoldingRegisters(0x0000, 1);
    assert(result == ModbusMaster::ku8MBSuccess);

    // Pre/post transmission callbacks
    int preCalled = 0, postCalled = 0;
    node.preTransmission([&]() { preCalled++; });
    node.postTransmission([&]() { postCalled++; });
    node.readHoldingRegisters(0x0000, 1);
    assert(preCalled == 1);
    assert(postCalled == 1);

    printf("test_modbusmaster: all assertions passed\n");
    return 0;
}
