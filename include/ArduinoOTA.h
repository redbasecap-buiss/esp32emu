#pragma once
// esp32emu — ArduinoOTA mock
// Simulates OTA update lifecycle without actual firmware flashing.

#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>

enum ota_error_t {
    OTA_AUTH_ERROR = 0,
    OTA_BEGIN_ERROR,
    OTA_CONNECT_ERROR,
    OTA_RECEIVE_ERROR,
    OTA_END_ERROR
};

class ArduinoOTAClass {
public:
    static ArduinoOTAClass& instance() {
        static ArduinoOTAClass inst;
        return inst;
    }

    ArduinoOTAClass& setHostname(const char* hostname) {
        hostname_ = hostname ? hostname : "";
        return *this;
    }

    ArduinoOTAClass& setPassword(const char* password) {
        password_ = password ? password : "";
        return *this;
    }

    ArduinoOTAClass& setPasswordHash(const char* hash) {
        (void)hash;
        return *this;
    }

    ArduinoOTAClass& setPort(uint16_t port) {
        port_ = port;
        return *this;
    }

    ArduinoOTAClass& setRebootOnSuccess(bool reboot) {
        rebootOnSuccess_ = reboot;
        return *this;
    }

    void onStart(std::function<void()> fn) { onStart_ = fn; }
    void onEnd(std::function<void()> fn) { onEnd_ = fn; }
    void onProgress(std::function<void(unsigned int, unsigned int)> fn) { onProgress_ = fn; }
    void onError(std::function<void(ota_error_t)> fn) { onError_ = fn; }

    void begin() {
        started_ = true;
        fprintf(stderr, "[esp32emu] ArduinoOTA started: hostname='%s' port=%d\n",
                hostname_.c_str(), port_);
    }

    void handle() {
        // No-op in emulation; call simulateOTA() to trigger lifecycle
    }

    std::string getHostname() const { return hostname_; }

    // ── Test helpers ──

    bool isStarted() const { return started_; }

    // Simulate a complete OTA update cycle
    void simulateOTA(bool success = true) {
        if (!started_) return;
        fprintf(stderr, "[esp32emu] OTA update simulation %s\n", success ? "started" : "will fail");

        if (onStart_) onStart_();

        if (success) {
            unsigned int total = 100000;
            for (unsigned int i = 0; i <= total; i += 25000) {
                if (onProgress_) onProgress_(i, total);
            }
            if (onEnd_) onEnd_();
            fprintf(stderr, "[esp32emu] OTA update complete%s\n",
                    rebootOnSuccess_ ? " (would reboot)" : "");
        } else {
            if (onError_) onError_(OTA_RECEIVE_ERROR);
            fprintf(stderr, "[esp32emu] OTA update failed\n");
        }
    }

private:
    std::string hostname_ = "esp32-emu";
    std::string password_;
    uint16_t port_ = 3232;
    bool started_ = false;
    bool rebootOnSuccess_ = true;
    std::function<void()> onStart_;
    std::function<void()> onEnd_;
    std::function<void(unsigned int, unsigned int)> onProgress_;
    std::function<void(ota_error_t)> onError_;
};

static ArduinoOTAClass& ArduinoOTA = ArduinoOTAClass::instance();
