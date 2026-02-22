#pragma once
// esp32emu — MCP2515 CAN bus controller mock

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace MCP2515_NS {
    enum ERROR : uint8_t { ERROR_OK = 0, ERROR_FAIL = 1, ERROR_NOMSG = 3 };
    enum CAN_SPEED : uint8_t { CAN_125KBPS = 0, CAN_250KBPS, CAN_500KBPS, CAN_1000KBPS };
    enum CAN_CLOCK : uint8_t { MCP_8MHZ = 0, MCP_16MHZ };
    enum MASK : uint8_t { MASK0 = 0, MASK1 };
    enum RXF : uint8_t { RXF0 = 0, RXF1, RXF2, RXF3, RXF4, RXF5 };
    enum CANINTF : uint8_t { RX0IF = 0x01, RX1IF = 0x02 };
}

struct can_frame {
    uint32_t can_id = 0;
    uint8_t  can_dlc = 0;
    uint8_t  data[8] = {};
};

class MCP2515 {
public:
    using ERROR = MCP2515_NS::ERROR;
    using CAN_SPEED = MCP2515_NS::CAN_SPEED;
    using CAN_CLOCK = MCP2515_NS::CAN_CLOCK;

    MCP2515(int cs_pin) : cs_pin_(cs_pin) {
        fprintf(stderr, "[esp32emu] MCP2515: CS pin %d\n", cs_pin);
    }

    ERROR reset() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] MCP2515: reset\n");
        return MCP2515_NS::ERROR_OK;
    }

    ERROR setBitrate(CAN_SPEED speed, CAN_CLOCK clock = MCP2515_NS::MCP_8MHZ) {
        speed_ = speed;
        clock_ = clock;
        fprintf(stderr, "[esp32emu] MCP2515: bitrate set\n");
        return MCP2515_NS::ERROR_OK;
    }

    ERROR setNormalMode() { mode_ = 0; return MCP2515_NS::ERROR_OK; }
    ERROR setLoopbackMode() { mode_ = 1; return MCP2515_NS::ERROR_OK; }
    ERROR setListenOnlyMode() { mode_ = 2; return MCP2515_NS::ERROR_OK; }

    ERROR sendMessage(const can_frame* frame) {
        if (!initialized_ || !frame) return MCP2515_NS::ERROR_FAIL;
        tx_count_++;
        if (mode_ == 1) { // loopback
            rx_queue_.push_back(*frame);
        }
        fprintf(stderr, "[esp32emu] MCP2515: TX id=0x%03X dlc=%d\n", frame->can_id, frame->can_dlc);
        return MCP2515_NS::ERROR_OK;
    }

    ERROR readMessage(can_frame* frame) {
        if (!frame || rx_queue_.empty()) return MCP2515_NS::ERROR_NOMSG;
        *frame = rx_queue_.front();
        rx_queue_.erase(rx_queue_.begin());
        return MCP2515_NS::ERROR_OK;
    }

    bool checkReceive() { return !rx_queue_.empty(); }

    ERROR setFilterMask(MCP2515_NS::MASK mask, bool ext, uint32_t val) {
        (void)mask; (void)ext; (void)val;
        return MCP2515_NS::ERROR_OK;
    }
    ERROR setFilter(MCP2515_NS::RXF rxf, bool ext, uint32_t val) {
        (void)rxf; (void)ext; (void)val;
        return MCP2515_NS::ERROR_OK;
    }

    uint8_t getStatus() { return rx_queue_.empty() ? 0 : MCP2515_NS::RX0IF; }
    uint8_t errorCountRX() { return rx_err_; }
    uint8_t errorCountTX() { return tx_err_; }

    // Test helpers
    void _injectFrame(const can_frame& f) { rx_queue_.push_back(f); }
    int  _getTxCount() const { return tx_count_; }
    bool _isInitialized() const { return initialized_; }
    int  _getMode() const { return mode_; }
    void _setErrorCountRX(uint8_t v) { rx_err_ = v; }
    void _setErrorCountTX(uint8_t v) { tx_err_ = v; }

private:
    int cs_pin_;
    bool initialized_ = false;
    int mode_ = 0;
    CAN_SPEED speed_ = MCP2515_NS::CAN_500KBPS;
    CAN_CLOCK clock_ = MCP2515_NS::MCP_8MHZ;
    int tx_count_ = 0;
    uint8_t rx_err_ = 0;
    uint8_t tx_err_ = 0;
    std::vector<can_frame> rx_queue_;
};
