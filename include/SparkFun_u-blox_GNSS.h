// esp32emu — SparkFun u-blox GNSS mock: GPS/GNSS module (I2C/Serial)
#pragma once
#include <cstdint>
#include <cstdio>

class SFE_UBLOX_GNSS {
public:
    SFE_UBLOX_GNSS()
        : _begun(false), _lat(471234567), _lon(85432100), _alt(450000),
          _altMSL(445000), _speed(0), _heading(0), _siv(8),
          _fixType(3), _year(2026), _month(2), _day(22),
          _hour(12), _minute(0), _second(0), _pDOP(150) {}

    bool begin(uint8_t addr = 0x42) {
        (void)addr;
        _begun = true;
        printf("[esp32emu] u-blox GNSS: initialized (I2C 0x%02X)\n", addr);
        return true;
    }

    bool begin(void* serial) {
        (void)serial;
        _begun = true;
        printf("[esp32emu] u-blox GNSS: initialized (Serial)\n");
        return true;
    }

    void setI2COutput(uint8_t mask) { (void)mask; }
    void setNavigationFrequency(uint8_t hz) { _navHz = hz; }
    void setAutoPVT(bool enable) { _autoPVT = enable; }

    bool getPVT() { return _begun; }

    int32_t getLatitude() const { return _lat; }
    int32_t getLongitude() const { return _lon; }
    int32_t getAltitude() const { return _alt; }
    int32_t getAltitudeMSL() const { return _altMSL; }
    int32_t getGroundSpeed() const { return _speed; }     // mm/s
    int32_t getHeading() const { return _heading; }        // degrees * 10^-5
    uint8_t getSIV() const { return _siv; }                // satellites in view
    uint8_t getFixType() const { return _fixType; }        // 0=none, 2=2D, 3=3D
    uint16_t getPDOP() const { return _pDOP; }

    uint16_t getYear() const { return _year; }
    uint8_t getMonth() const { return _month; }
    uint8_t getDay() const { return _day; }
    uint8_t getHour() const { return _hour; }
    uint8_t getMinute() const { return _minute; }
    uint8_t getSecond() const { return _second; }

    bool getDateValid() const { return _fixType >= 2; }
    bool getTimeValid() const { return _fixType >= 2; }

    void factoryDefault() {
        printf("[esp32emu] u-blox GNSS: factory default\n");
    }

    // --- Test helpers ---
    void test_setPosition(int32_t lat, int32_t lon, int32_t alt) {
        _lat = lat; _lon = lon; _alt = alt;
    }
    void test_setSpeed(int32_t speed, int32_t heading) {
        _speed = speed; _heading = heading;
    }
    void test_setSIV(uint8_t siv) { _siv = siv; }
    void test_setFixType(uint8_t ft) { _fixType = ft; }
    void test_setDateTime(uint16_t y, uint8_t mo, uint8_t d, uint8_t h, uint8_t mi, uint8_t s) {
        _year = y; _month = mo; _day = d; _hour = h; _minute = mi; _second = s;
    }
    bool test_isBegun() const { return _begun; }

private:
    bool _begun;
    int32_t _lat, _lon, _alt, _altMSL;
    int32_t _speed, _heading;
    uint8_t _siv, _fixType;
    uint16_t _year; uint8_t _month, _day, _hour, _minute, _second;
    uint16_t _pDOP;
    uint8_t _navHz = 1;
    bool _autoPVT = false;
};
