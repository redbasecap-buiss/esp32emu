#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <functional>
#include "esp32emu_string.h"

#define WL_CONNECTED    3
#define WL_DISCONNECTED 6
#define WL_IDLE_STATUS  0
#define WIFI_STA        1
#define WIFI_AP         2
#define WIFI_AP_STA     3

class IPAddress {
public:
    IPAddress() : addr_{127,0,0,1} {}
    IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : addr_{a,b,c,d} {}

    uint8_t operator[](int i) const { return addr_[i]; }
    operator uint32_t() const {
        return ((uint32_t)addr_[0]) | ((uint32_t)addr_[1]<<8) |
               ((uint32_t)addr_[2]<<16) | ((uint32_t)addr_[3]<<24);
    }

    String toString() const {
        char buf[20];
        snprintf(buf, sizeof(buf), "%d.%d.%d.%d", addr_[0], addr_[1], addr_[2], addr_[3]);
        return String(buf);
    }

    // Allow printing
    friend class HardwareSerial;

private:
    uint8_t addr_[4];
};

class WiFiClass {
public:
    void mode(int m) { mode_ = m; }

    int begin(const char* ssid, const char* pass = nullptr) {
        if (!wifi_check_()) return WL_DISCONNECTED;
        ssid_ = ssid ? ssid : "";
        (void)pass;
        connected_ = true;
        return WL_CONNECTED;
    }

    void disconnect(bool wifioff = false) {
        (void)wifioff;
        connected_ = false;
    }

    int status() { return connected_ ? WL_CONNECTED : WL_DISCONNECTED; }

    IPAddress localIP()    { return IPAddress(127,0,0,1); }
    IPAddress gatewayIP()  { return IPAddress(127,0,0,1); }
    IPAddress subnetMask() { return IPAddress(255,255,255,0); }

    String SSID()   { return String(ssid_.c_str()); }
    int32_t RSSI()  { return -42; }
    String macAddress() { return String("DE:AD:BE:EF:00:01"); }

    void setHostname(const char* name) { hostname_ = name ? name : ""; }
    const char* getHostname() { return hostname_.c_str(); }

    bool softAP(const char* ssid, const char* pass = nullptr) {
        (void)ssid; (void)pass; return true;
    }
    IPAddress softAPIP() { return IPAddress(192,168,4,1); }

    // Board WiFi check callback (set by board init)
    std::function<bool()> wifi_check_ = []() { return true; };

public:
    void setWiFiCheck(std::function<bool()> fn) { wifi_check_ = fn; }

private:
    bool connected_ = false;
    int mode_ = WIFI_STA;
    std::string ssid_;
    std::string hostname_ = "esp32emu";
};

extern WiFiClass WiFi;
