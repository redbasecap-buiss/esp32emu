// esp32emu — USB mock: ESP32-S2/S3 USB stack
#pragma once
#include <cstdio>
#include <string>

class ESPUSB {
public:
    ESPUSB() : _begun(false) {}

    bool begin() {
        _begun = true;
        printf("[esp32emu] USB: initialized\n");
        return true;
    }

    void end() { _begun = false; }

    void manufacturerName(const char* name) { _manufacturer = name ? name : ""; }
    void productName(const char* name) { _product = name ? name : ""; }
    void serialNumber(const char* sn) { _serial = sn ? sn : ""; }
    void firmwareVersion(uint16_t v) { _fwVersion = v; }

    bool isBegun() const { return _begun; }
    const std::string& getManufacturer() const { return _manufacturer; }
    const std::string& getProduct() const { return _product; }
    const std::string& getSerial() const { return _serial; }
    uint16_t getFirmwareVersion() const { return _fwVersion; }

private:
    bool _begun;
    std::string _manufacturer;
    std::string _product;
    std::string _serial;
    uint16_t _fwVersion = 0x0100;
};

inline ESPUSB USB;
