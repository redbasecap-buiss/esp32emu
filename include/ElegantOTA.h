#pragma once
#include <cstdio>
#include <functional>
#include "WebServer.h"

class ElegantOTAClass {
public:
    void begin(WebServer* server, const char* username = "", const char* password = "") {
        _server = server;
        _username = username ? username : "";
        _password = password ? password : "";
        _running = true;
        fprintf(stderr, "[esp32emu] ElegantOTA: started on /update\n");
    }

    void loop() {
        // No-op in emulation
    }

    void setAutoReboot(bool enable) { _autoReboot = enable; }

    void onStart(std::function<void()> fn) { _onStart = fn; }
    void onEnd(std::function<void(bool success)> fn) { _onEnd = fn; }
    void onProgress(std::function<void(size_t current, size_t total)> fn) { _onProgress = fn; }

    // Test helper: simulate an OTA update
    void _simulateUpdate(bool success = true) {
        if (_onStart) _onStart();
        if (_onProgress) {
            _onProgress(0, 100);
            _onProgress(50, 100);
            _onProgress(100, 100);
        }
        if (_onEnd) _onEnd(success);
        fprintf(stderr, "[esp32emu] ElegantOTA: update %s\n", success ? "succeeded" : "failed");
    }

    bool isRunning() const { return _running; }

private:
    WebServer* _server = nullptr;
    std::string _username;
    std::string _password;
    bool _running = false;
    bool _autoReboot = true;
    std::function<void()> _onStart;
    std::function<void(bool)> _onEnd;
    std::function<void(size_t, size_t)> _onProgress;
};

extern ElegantOTAClass ElegantOTA;
