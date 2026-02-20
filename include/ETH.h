#pragma once
// esp32emu: Ethernet library mock for ESP32
#include <cstdint>
#include <string>
#include "Arduino.h"
#include "WiFi.h"  // Reuse IPAddress etc.

typedef enum {
    ETH_PHY_LAN8720,
    ETH_PHY_TLK110,
    ETH_PHY_RTL8201,
    ETH_PHY_DP83848,
    ETH_PHY_DM9051,
    ETH_PHY_KSZ8041,
    ETH_PHY_KSZ8081,
    ETH_PHY_W5500,
    ETH_PHY_MAX
} eth_phy_type_t;

typedef enum {
    ETH_CLOCK_GPIO0_IN,
    ETH_CLOCK_GPIO0_OUT,
    ETH_CLOCK_GPIO16_OUT,
    ETH_CLOCK_GPIO17_OUT,
} eth_clock_mode_t;

class ETHClass {
public:
    bool begin(uint8_t phy_addr = 0, int power = -1, int mdc = 23, int mdio = 18,
               eth_phy_type_t type = ETH_PHY_LAN8720, eth_clock_mode_t clk = ETH_CLOCK_GPIO0_IN) {
        (void)phy_addr; (void)power; (void)mdc; (void)mdio; (void)type; (void)clk;
        _started = true;
        _connected = true;
        _linkUp = true;
        return true;
    }

    bool config(uint32_t local_ip, uint32_t gateway, uint32_t subnet, uint32_t dns1 = 0, uint32_t dns2 = 0) {
        (void)local_ip; (void)gateway; (void)subnet; (void)dns1; (void)dns2;
        _staticIP = true;
        return true;
    }

    bool linkUp() const { return _linkUp; }
    bool connected() const { return _connected; }
    uint8_t linkSpeed() const { return _linkSpeed; }
    bool fullDuplex() const { return _fullDuplex; }

    IPAddress localIP() const { return IPAddress(192, 168, 1, 100); }
    IPAddress subnetMask() const { return IPAddress(255, 255, 255, 0); }
    IPAddress gatewayIP() const { return IPAddress(192, 168, 1, 1); }
    IPAddress dnsIP(uint8_t idx = 0) const { (void)idx; return IPAddress(8, 8, 8, 8); }

    String macAddress() const { return "AA:BB:CC:DD:EE:FF"; }
    const char* getHostname() const { return _hostname.c_str(); }
    bool setHostname(const char* hostname) { _hostname = hostname ? hostname : ""; return true; }

    // Test helpers
    void setLinkUp(bool up) { _linkUp = up; _connected = up; }
    void setLinkSpeed(uint8_t speed) { _linkSpeed = speed; }
    void setFullDuplex(bool fd) { _fullDuplex = fd; }
    void reset() { _started = false; _connected = false; _linkUp = false; _staticIP = false; _linkSpeed = 100; _fullDuplex = true; _hostname = "esp32"; }

private:
    bool _started = false;
    bool _connected = false;
    bool _linkUp = false;
    bool _staticIP = false;
    uint8_t _linkSpeed = 100;
    bool _fullDuplex = true;
    std::string _hostname = "esp32";
};

extern ETHClass ETH;
