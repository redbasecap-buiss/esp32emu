#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
#include <queue>
#include <cstring>

class SoftwareSerial {
public:
    SoftwareSerial(uint8_t rx, uint8_t tx, bool inverse_logic = false) 
        : rx_pin_(rx), tx_pin_(tx) {
        (void)inverse_logic;
    }

    void begin(long baud) {
        fprintf(stderr, "[esp32emu] SoftwareSerial on pins RX=%d TX=%d @ %ld baud\n",
                rx_pin_, tx_pin_, baud);
    }

    void end() {}

    size_t write(uint8_t c) {
        fprintf(stdout, "%c", c);
        return 1;
    }

    size_t write(const uint8_t* buf, size_t len) {
        for (size_t i = 0; i < len; i++) write(buf[i]);
        return len;
    }

    size_t print(const char* s) { return write((const uint8_t*)s, strlen(s)); }
    size_t println(const char* s = "") { print(s); write('\n'); return strlen(s) + 1; }
    size_t print(int n) { return print(std::to_string(n).c_str()); }
    size_t println(int n) { auto s = std::to_string(n); print(s.c_str()); write('\n'); return s.size() + 1; }

    int available() { return (int)rx_buf_.size(); }
    int read() {
        if (rx_buf_.empty()) return -1;
        int c = rx_buf_.front();
        rx_buf_.pop();
        return c;
    }
    int peek() { return rx_buf_.empty() ? -1 : rx_buf_.front(); }
    void flush() {}
    bool listen() { return true; }
    bool isListening() { return true; }
    operator bool() { return true; }

    // Emulator-only: inject data
    void injectByte(uint8_t c) { rx_buf_.push(c); }

private:
    uint8_t rx_pin_, tx_pin_;
    std::queue<uint8_t> rx_buf_;
};
