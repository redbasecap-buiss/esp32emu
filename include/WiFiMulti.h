#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "WiFi.h"

class WiFiMulti {
public:
    bool addAP(const char* ssid, const char* passphrase = nullptr) {
        _aps.push_back({ssid, passphrase ? passphrase : ""});
        return true;
    }

    uint8_t run(uint32_t connectTimeout = 5000) {
        (void)connectTimeout;
        if (_aps.empty()) return WL_DISCONNECTED;
        // Simulate connecting to the first AP
        WiFi.begin(_aps[0].ssid.c_str(), _aps[0].pass.c_str());
        _connected_index = 0;
        return WL_CONNECTED;
    }

    int16_t connectedIndex() const { return _connected_index; }
    size_t APcount() const { return _aps.size(); }

private:
    struct AP {
        std::string ssid;
        std::string pass;
    };
    std::vector<AP> _aps;
    int16_t _connected_index = -1;
};
