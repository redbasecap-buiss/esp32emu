#pragma once
// esp32emu — AsyncTCP mock (me-no-dev/AsyncTCP compatible)
// Provides async TCP client/server used by ESPAsyncWebServer and other async libs.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

class AsyncClient;

typedef std::function<void(void*, AsyncClient*)> AcConnectHandler;
typedef std::function<void(void*, AsyncClient*, size_t len, uint32_t time)> AcAckHandler;
typedef std::function<void(void*, AsyncClient*, int8_t error)> AcErrorHandler;
typedef std::function<void(void*, AsyncClient*, void* data, size_t len)> AcDataHandler;
typedef std::function<void(void*, AsyncClient*, uint32_t time)> AcTimeoutHandler;

class AsyncClient {
public:
    AsyncClient() = default;
    ~AsyncClient() = default;

    bool connect(const char* host, uint16_t port) {
        printf("[esp32emu] AsyncTCP: connect to %s:%u\n", host, port);
        host_ = host ? host : "";
        port_ = port;
        connected_ = !testFailConnect_;
        if (connected_ && connectCb_) connectCb_(connectArg_, this);
        return connected_;
    }

    bool connected() const { return connected_; }
    void close(bool now = false) {
        (void)now;
        connected_ = false;
        if (disconnectCb_) disconnectCb_(disconnectArg_, this);
    }

    size_t write(const char* data, size_t len) {
        if (!connected_) return 0;
        txBuffer_.append(data, len);
        printf("[esp32emu] AsyncTCP: wrote %zu bytes\n", len);
        return len;
    }

    size_t write(const char* data) {
        return data ? write(data, strlen(data)) : 0;
    }

    size_t space() const { return 1460; }
    bool canSend() const { return connected_; }
    void setRxTimeout(uint32_t t) { rxTimeout_ = t; }
    void setAckTimeout(uint32_t t) { ackTimeout_ = t; }
    void setNoDelay(bool nd) { noDelay_ = nd; }
    bool getNoDelay() const { return noDelay_; }

    uint16_t remotePort() const { return port_; }
    uint32_t remoteIP() const { return 0x7F000001; } // 127.0.0.1

    void onConnect(AcConnectHandler cb, void* arg = nullptr) { connectCb_ = cb; connectArg_ = arg; }
    void onDisconnect(AcConnectHandler cb, void* arg = nullptr) { disconnectCb_ = cb; disconnectArg_ = arg; }
    void onData(AcDataHandler cb, void* arg = nullptr) { dataCb_ = cb; dataArg_ = arg; }
    void onError(AcErrorHandler cb, void* arg = nullptr) { errorCb_ = cb; errorArg_ = arg; }
    void onAck(AcAckHandler cb, void* arg = nullptr) { ackCb_ = cb; ackArg_ = arg; }
    void onTimeout(AcTimeoutHandler cb, void* arg = nullptr) { timeoutCb_ = cb; timeoutArg_ = arg; }

    // Test helpers
    void testInjectData(const void* data, size_t len) {
        if (dataCb_) dataCb_(dataArg_, this, const_cast<void*>(data), len);
    }
    void testSetFailConnect(bool fail) { testFailConnect_ = fail; }
    const std::string& testGetTxBuffer() const { return txBuffer_; }
    void testClearTxBuffer() { txBuffer_.clear(); }

private:
    std::string host_;
    uint16_t port_ = 0;
    bool connected_ = false;
    bool noDelay_ = false;
    bool testFailConnect_ = false;
    uint32_t rxTimeout_ = 0;
    uint32_t ackTimeout_ = 0;
    std::string txBuffer_;

    AcConnectHandler connectCb_;     void* connectArg_ = nullptr;
    AcConnectHandler disconnectCb_;  void* disconnectArg_ = nullptr;
    AcDataHandler dataCb_;           void* dataArg_ = nullptr;
    AcErrorHandler errorCb_;         void* errorArg_ = nullptr;
    AcAckHandler ackCb_;             void* ackArg_ = nullptr;
    AcTimeoutHandler timeoutCb_;     void* timeoutArg_ = nullptr;
};

class AsyncServer {
public:
    explicit AsyncServer(uint16_t port) : port_(port) {}

    void begin() {
        running_ = true;
        printf("[esp32emu] AsyncTCP server: listening on port %u\n", port_);
    }

    void end() {
        running_ = false;
        printf("[esp32emu] AsyncTCP server: stopped\n");
    }

    void onClient(std::function<void(void*, AsyncClient*)> cb, void* arg = nullptr) {
        clientCb_ = cb;
        clientArg_ = arg;
    }

    bool running() const { return running_; }
    uint16_t port() const { return port_; }

    // Test helper: simulate incoming client connection
    void testInjectClient(AsyncClient* client) {
        if (clientCb_) clientCb_(clientArg_, client);
    }

private:
    uint16_t port_;
    bool running_ = false;
    std::function<void(void*, AsyncClient*)> clientCb_;
    void* clientArg_ = nullptr;
};
