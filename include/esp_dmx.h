#pragma once
// esp32emu — ESP-DMX mock (DMX-512 via RS-485)

#include <cstdint>
#include <cstdio>
#include <cstring>

#define DMX_MAX_PACKET_SIZE 513  // 1 start code + 512 channels
#define DMX_OK              0
#define DMX_ERR_TIMEOUT     1
#define DMX_ERR_NOT_INIT    2

// DMX personality types
#define DMX_TYPE_SEND   0
#define DMX_TYPE_RECEIVE 1

namespace dmx {

struct Config {
    int uart_num = 1;
    int tx_pin = 17;
    int rx_pin = 16;
    int en_pin = 21;
    int type = DMX_TYPE_SEND;
};

class DMX {
public:
    DMX() { memset(_data, 0, sizeof(_data)); }

    int install(const Config& cfg) {
        _config = cfg;
        _installed = true;
        printf("[esp32emu] DMX: install(uart=%d, type=%s)\n",
               cfg.uart_num, cfg.type == DMX_TYPE_SEND ? "SEND" : "RECEIVE");
        return DMX_OK;
    }

    int uninstall() {
        _installed = false;
        printf("[esp32emu] DMX: uninstall()\n");
        return DMX_OK;
    }

    // Write a single channel (1-512)
    void write(uint16_t channel, uint8_t value) {
        if (channel >= 1 && channel <= 512) {
            _data[channel] = value;
        }
    }

    // Read a single channel (1-512)
    uint8_t read(uint16_t channel) const {
        if (channel >= 1 && channel <= 512) return _data[channel];
        return 0;
    }

    // Write entire buffer
    void writeAll(const uint8_t* data, size_t len) {
        size_t n = len < DMX_MAX_PACKET_SIZE ? len : DMX_MAX_PACKET_SIZE;
        memcpy(_data, data, n);
    }

    // Read entire buffer
    void readAll(uint8_t* data, size_t len) const {
        size_t n = len < DMX_MAX_PACKET_SIZE ? len : DMX_MAX_PACKET_SIZE;
        memcpy(data, _data, n);
    }

    // Send the current packet
    int send() {
        if (!_installed) return DMX_ERR_NOT_INIT;
        _packetsSent++;
        printf("[esp32emu] DMX: send() packet #%u\n", _packetsSent);
        return DMX_OK;
    }

    // Receive a packet (non-blocking)
    int receive() {
        if (!_installed) return DMX_ERR_NOT_INIT;
        if (_hasIncoming) {
            _hasIncoming = false;
            return DMX_OK;
        }
        return DMX_ERR_TIMEOUT;
    }

    // Set start code
    void setStartCode(uint8_t code) { _data[0] = code; }
    uint8_t getStartCode() const { return _data[0]; }

    bool isInstalled() const { return _installed; }

    // === Test helpers ===
    void emu_setChannelData(uint16_t channel, uint8_t value) {
        if (channel >= 1 && channel <= 512) _data[channel] = value;
    }

    void emu_injectPacket(const uint8_t* data, size_t len) {
        size_t n = len < DMX_MAX_PACKET_SIZE ? len : DMX_MAX_PACKET_SIZE;
        memcpy(_data, data, n);
        _hasIncoming = true;
    }

    uint32_t emu_getPacketsSent() const { return _packetsSent; }
    uint8_t emu_getChannel(uint16_t ch) const { return read(ch); }

private:
    Config _config;
    bool _installed = false;
    bool _hasIncoming = false;
    uint8_t _data[DMX_MAX_PACKET_SIZE] = {};
    uint32_t _packetsSent = 0;
};

} // namespace dmx
