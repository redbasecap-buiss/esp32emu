#pragma once
// esp32emu — BluetoothSerial mock
// Simulates ESP32 Bluetooth Serial SPP with an internal ring buffer.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <mutex>
#include <functional>

class BluetoothSerial {
public:
    typedef std::function<void(const uint8_t* buf, size_t len)> DataCallback;

    bool begin(const char* name = "ESP32", bool isMaster = false) {
        (void)isMaster;
        name_ = name ? name : "ESP32";
        started_ = true;
        fprintf(stderr, "[esp32emu] BluetoothSerial started: '%s'\n", name_.c_str());
        return true;
    }

    bool begin(const std::string& name, bool isMaster = false) {
        return begin(name.c_str(), isMaster);
    }

    void end() {
        started_ = false;
        rxBuf_.clear();
        fprintf(stderr, "[esp32emu] BluetoothSerial stopped\n");
    }

    bool hasClient() const { return connected_; }
    bool isReady() const { return started_; }

    int available() {
        std::lock_guard<std::mutex> lk(mtx_);
        return (int)rxBuf_.size();
    }

    int read() {
        std::lock_guard<std::mutex> lk(mtx_);
        if (rxBuf_.empty()) return -1;
        uint8_t c = rxBuf_.front();
        rxBuf_.erase(rxBuf_.begin());
        return c;
    }

    size_t readBytes(uint8_t* buf, size_t len) {
        std::lock_guard<std::mutex> lk(mtx_);
        size_t n = std::min(len, rxBuf_.size());
        std::memcpy(buf, rxBuf_.data(), n);
        rxBuf_.erase(rxBuf_.begin(), rxBuf_.begin() + n);
        return n;
    }

    std::string readString() {
        std::lock_guard<std::mutex> lk(mtx_);
        std::string s(rxBuf_.begin(), rxBuf_.end());
        rxBuf_.clear();
        return s;
    }

    std::string readStringUntil(char term) {
        std::lock_guard<std::mutex> lk(mtx_);
        std::string s;
        while (!rxBuf_.empty()) {
            char c = rxBuf_.front();
            rxBuf_.erase(rxBuf_.begin());
            if (c == term) break;
            s += c;
        }
        return s;
    }

    size_t write(uint8_t c) {
        std::lock_guard<std::mutex> lk(mtx_);
        txBuf_.push_back(c);
        return 1;
    }

    size_t write(const uint8_t* buf, size_t len) {
        std::lock_guard<std::mutex> lk(mtx_);
        txBuf_.insert(txBuf_.end(), buf, buf + len);
        return len;
    }

    size_t print(const char* s) { return write((const uint8_t*)s, strlen(s)); }
    size_t println(const char* s) { size_t n = print(s); n += print("\r\n"); return n; }
    size_t print(int v) { return print(std::to_string(v).c_str()); }
    size_t println(int v) { return println(std::to_string(v).c_str()); }
    size_t println() { return print("\r\n"); }

    void flush() {}

    bool disconnect() {
        connected_ = false;
        return true;
    }

    bool connect(const char* remoteName) {
        (void)remoteName;
        connected_ = true;
        fprintf(stderr, "[esp32emu] BluetoothSerial connected to '%s'\n", remoteName);
        return true;
    }

    void register_callback(DataCallback cb) { dataCb_ = cb; }

    // ── Test helpers ──

    // Inject data as if received from BT peer
    void injectRx(const char* data) {
        std::lock_guard<std::mutex> lk(mtx_);
        const uint8_t* p = (const uint8_t*)data;
        rxBuf_.insert(rxBuf_.end(), p, p + strlen(data));
    }

    void injectRx(const uint8_t* data, size_t len) {
        std::lock_guard<std::mutex> lk(mtx_);
        rxBuf_.insert(rxBuf_.end(), data, data + len);
    }

    // Simulate a peer connection
    void simulateConnect() { connected_ = true; }
    void simulateDisconnect() { connected_ = false; }

    // Read what the sketch wrote (TX)
    std::string getTxData() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return std::string(txBuf_.begin(), txBuf_.end());
    }

    void clearTx() {
        std::lock_guard<std::mutex> lk(mtx_);
        txBuf_.clear();
    }

private:
    std::string name_ = "ESP32";
    bool started_ = false;
    bool connected_ = false;
    std::vector<uint8_t> rxBuf_;
    std::vector<uint8_t> txBuf_;
    DataCallback dataCb_;
    mutable std::mutex mtx_;
};
