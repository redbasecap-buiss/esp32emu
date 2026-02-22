#pragma once
// esp32emu — ModbusMaster mock (Modbus RTU master for RS-485)

#include <cstdint>
#include <cstdio>
#include <map>
#include <functional>

// Forward declarations
class HardwareSerial;

class ModbusMaster {
public:
    // Result codes
    static constexpr uint8_t ku8MBSuccess = 0x00;
    static constexpr uint8_t ku8MBInvalidSlaveID = 0xE0;
    static constexpr uint8_t ku8MBInvalidFunction = 0xE1;
    static constexpr uint8_t ku8MBResponseTimedOut = 0xE2;
    static constexpr uint8_t ku8MBInvalidCRC = 0xE3;

    ModbusMaster() = default;

    void begin(uint8_t slaveId, HardwareSerial& /*serial*/) {
        _slaveId = slaveId;
        printf("[esp32emu] ModbusMaster: begin(slave=%u)\n", slaveId);
    }

    // Pre/post transmission callbacks (for DE/RE pin toggling)
    void preTransmission(std::function<void()> fn) { _preTx = fn; }
    void postTransmission(std::function<void()> fn) { _postTx = fn; }

    // Read holding registers (FC 0x03)
    uint8_t readHoldingRegisters(uint16_t addr, uint16_t qty) {
        if (_preTx) _preTx();
        printf("[esp32emu] Modbus: readHoldingRegisters(0x%04X, %u)\n", addr, qty);
        _responseQty = qty;
        _responseAddr = addr;
        if (_postTx) _postTx();
        if (_failNext) { _failNext = false; return ku8MBResponseTimedOut; }
        return ku8MBSuccess;
    }

    // Read input registers (FC 0x04)
    uint8_t readInputRegisters(uint16_t addr, uint16_t qty) {
        if (_preTx) _preTx();
        printf("[esp32emu] Modbus: readInputRegisters(0x%04X, %u)\n", addr, qty);
        _responseQty = qty;
        _responseAddr = addr;
        if (_postTx) _postTx();
        if (_failNext) { _failNext = false; return ku8MBResponseTimedOut; }
        return ku8MBSuccess;
    }

    // Write single register (FC 0x06)
    uint8_t writeSingleRegister(uint16_t addr, uint16_t val) {
        if (_preTx) _preTx();
        printf("[esp32emu] Modbus: writeSingleRegister(0x%04X, %u)\n", addr, val);
        _registers[addr] = val;
        if (_postTx) _postTx();
        if (_failNext) { _failNext = false; return ku8MBResponseTimedOut; }
        return ku8MBSuccess;
    }

    // Write multiple registers (FC 0x10)
    uint8_t writeMultipleRegisters(uint16_t addr, uint16_t qty) {
        if (_preTx) _preTx();
        printf("[esp32emu] Modbus: writeMultipleRegisters(0x%04X, %u)\n", addr, qty);
        for (uint16_t i = 0; i < qty && i < 64; i++) {
            _registers[addr + i] = _txBuffer[i];
        }
        if (_postTx) _postTx();
        if (_failNext) { _failNext = false; return ku8MBResponseTimedOut; }
        return ku8MBSuccess;
    }

    // Read coils (FC 0x01)
    uint8_t readCoils(uint16_t addr, uint16_t qty) {
        if (_preTx) _preTx();
        printf("[esp32emu] Modbus: readCoils(0x%04X, %u)\n", addr, qty);
        _responseQty = qty;
        _responseAddr = addr;
        if (_postTx) _postTx();
        if (_failNext) { _failNext = false; return ku8MBResponseTimedOut; }
        return ku8MBSuccess;
    }

    // Write single coil (FC 0x05)
    uint8_t writeSingleCoil(uint16_t addr, uint8_t val) {
        if (_preTx) _preTx();
        printf("[esp32emu] Modbus: writeSingleCoil(0x%04X, %u)\n", addr, val);
        _coils[addr] = val;
        if (_postTx) _postTx();
        if (_failNext) { _failNext = false; return ku8MBResponseTimedOut; }
        return ku8MBSuccess;
    }

    // Get response data
    uint16_t getResponseBuffer(uint8_t index) const {
        uint16_t addr = _responseAddr + index;
        auto it = _registers.find(addr);
        return it != _registers.end() ? it->second : 0;
    }

    // Set transmit buffer for writeMultipleRegisters
    void setTransmitBuffer(uint8_t index, uint16_t value) {
        if (index < 64) _txBuffer[index] = value;
    }

    void clearTransmitBuffer() {
        for (int i = 0; i < 64; i++) _txBuffer[i] = 0;
    }

    void clearResponseBuffer() {}

    // === Test helpers ===
    void emu_setRegister(uint16_t addr, uint16_t val) { _registers[addr] = val; }
    uint16_t emu_getRegister(uint16_t addr) const {
        auto it = _registers.find(addr);
        return it != _registers.end() ? it->second : 0;
    }
    void emu_setCoil(uint16_t addr, uint8_t val) { _coils[addr] = val; }
    uint8_t emu_getCoil(uint16_t addr) const {
        auto it = _coils.find(addr);
        return it != _coils.end() ? it->second : 0;
    }
    void emu_setFailNext(bool f = true) { _failNext = f; }
    uint8_t emu_getSlaveId() const { return _slaveId; }

private:
    uint8_t _slaveId = 1;
    std::map<uint16_t, uint16_t> _registers;
    std::map<uint16_t, uint8_t> _coils;
    uint16_t _txBuffer[64] = {};
    uint16_t _responseAddr = 0;
    uint16_t _responseQty = 0;
    bool _failNext = false;
    std::function<void()> _preTx;
    std::function<void()> _postTx;
};
