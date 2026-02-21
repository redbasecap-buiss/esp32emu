#pragma once
// esp32emu — WiFiManager mock (tzapu/WiFiManager compatible)
// Simulates captive portal WiFi provisioning for host-side testing.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <vector>
#include "Arduino.h"

class WiFiManagerParameter {
public:
    WiFiManagerParameter(const char* id, const char* label, const char* defaultValue, int length)
        : id_(id ? id : ""), label_(label ? label : ""),
          value_(defaultValue ? defaultValue : ""), maxLen_(length) {}

    WiFiManagerParameter(const char* html)
        : customHTML_(html ? html : ""), maxLen_(0) {}

    const char* getID() const { return id_.c_str(); }
    const char* getLabel() const { return label_.c_str(); }
    const char* getValue() const { return value_.c_str(); }
    int getValueLength() const { return maxLen_; }
    const char* getCustomHTML() const { return customHTML_.c_str(); }

    // Test helper: inject a value as if user entered it in portal
    void setValue(const char* v) { value_ = v ? v : ""; }

private:
    std::string id_;
    std::string label_;
    std::string value_;
    std::string customHTML_;
    int maxLen_;
};

class WiFiManager {
public:
    WiFiManager() = default;

    // Main entry point — in emulator, always succeeds (simulates auto-connect)
    bool autoConnect(const char* apName = "ESP32_AP", const char* apPassword = nullptr) {
        printf("[esp32emu] WiFiManager: autoConnect('%s')\n", apName ? apName : "");
        apName_ = apName ? apName : "";
        if (apPassword) apPassword_ = apPassword;

        if (testForcePortal_) {
            printf("[esp32emu] WiFiManager: forced portal mode\n");
            if (apCallback_) apCallback_(this);
            // Simulate portal interaction
            portalRunning_ = true;
            if (saveCallback_) saveCallback_();
            portalRunning_ = false;
        }

        connected_ = !testFailConnect_;
        if (connected_) {
            printf("[esp32emu] WiFiManager: connected successfully\n");
        } else {
            printf("[esp32emu] WiFiManager: connection failed\n");
            if (failCallback_) failCallback_(this);
        }
        return connected_;
    }

    bool startConfigPortal(const char* apName = "ESP32_AP", const char* apPassword = nullptr) {
        printf("[esp32emu] WiFiManager: startConfigPortal('%s')\n", apName ? apName : "");
        apName_ = apName ? apName : "";
        if (apPassword) apPassword_ = apPassword;
        portalRunning_ = true;
        if (apCallback_) apCallback_(this);
        if (saveCallback_) saveCallback_();
        portalRunning_ = false;
        connected_ = !testFailConnect_;
        return connected_;
    }

    void resetSettings() {
        printf("[esp32emu] WiFiManager: settings reset\n");
        ssid_.clear();
        password_.clear();
        connected_ = false;
    }

    void setConfigPortalTimeout(unsigned long seconds) { portalTimeout_ = seconds; }
    void setConnectTimeout(unsigned long seconds) { connectTimeout_ = seconds; }
    void setTimeout(unsigned long seconds) { portalTimeout_ = seconds; }
    void setMinimumSignalQuality(int quality) { minSignalQuality_ = quality; }
    void setRemoveDuplicateAPs(bool remove) { removeDuplicates_ = remove; }
    void setDebugOutput(bool debug) { debugOutput_ = debug; }

    void setAPCallback(std::function<void(WiFiManager*)> cb) { apCallback_ = cb; }
    void setSaveConfigCallback(std::function<void()> cb) { saveCallback_ = cb; }
    void setConfigPortalBlocking(bool blocking) { blocking_ = blocking; }
    void setSaveParamsCallback(std::function<void()> cb) { saveParamsCallback_ = cb; }
    void setPreSaveConfigCallback(std::function<void()> cb) { preSaveCallback_ = cb; }

    // Called when connection fails
    void setAPClientFailCallback(std::function<void(WiFiManager*)> cb) { failCallback_ = cb; }

    void addParameter(WiFiManagerParameter* p) {
        if (p) params_.push_back(p);
    }

    WiFiManagerParameter** getParameters() {
        return params_.empty() ? nullptr : params_.data();
    }

    int getParametersCount() const { return static_cast<int>(params_.size()); }

    String getConfigPortalSSID() const { return String(apName_.c_str()); }
    bool getConfigPortalActive() const { return portalRunning_; }

    // Process portal (non-blocking mode)
    bool process() { return connected_; }

    // Test helpers
    void testSetForcePortal(bool force) { testForcePortal_ = force; }
    void testSetFailConnect(bool fail) { testFailConnect_ = fail; }
    void testSetSSID(const char* ssid) { ssid_ = ssid ? ssid : ""; }
    void testSetPassword(const char* pw) { password_ = pw ? pw : ""; }
    bool testIsConnected() const { return connected_; }

private:
    std::string apName_;
    std::string apPassword_;
    std::string ssid_;
    std::string password_;
    bool connected_ = false;
    bool portalRunning_ = false;
    bool blocking_ = true;
    bool debugOutput_ = true;
    bool removeDuplicates_ = true;
    bool testForcePortal_ = false;
    bool testFailConnect_ = false;
    unsigned long portalTimeout_ = 0;
    unsigned long connectTimeout_ = 0;
    int minSignalQuality_ = -1;

    std::function<void(WiFiManager*)> apCallback_;
    std::function<void()> saveCallback_;
    std::function<void()> saveParamsCallback_;
    std::function<void()> preSaveCallback_;
    std::function<void(WiFiManager*)> failCallback_;
    std::vector<WiFiManagerParameter*> params_;
};
