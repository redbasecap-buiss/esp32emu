#pragma once
// esp32emu — DS3231 RTC mock

#include <cstdint>
#include <cstdio>
#include <ctime>

struct DateTime {
    uint16_t year;
    uint8_t month, day, hour, minute, second, dayOfWeek;

    DateTime(uint16_t y = 2025, uint8_t mo = 1, uint8_t d = 1,
             uint8_t h = 0, uint8_t mi = 0, uint8_t s = 0)
        : year(y), month(mo), day(d), hour(h), minute(mi), second(s), dayOfWeek(0) {}

    uint32_t unixtime() const {
        struct tm t = {};
        t.tm_year = year - 1900;
        t.tm_mon = month - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = minute;
        t.tm_sec = second;
        return (uint32_t)mktime(&t);
    }
};

class DS3231 {
public:
    DS3231() = default;

    bool begin(void* /*wire*/ = nullptr) {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] DS3231: initialized\n");
        return true;
    }

    void adjust(const DateTime& dt) {
        now_ = dt;
        fprintf(stderr, "[esp32emu] DS3231: time set to %04d-%02d-%02d %02d:%02d:%02d\n",
                dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
    }

    DateTime now() const { return now_; }

    float getTemperature() const { return temp_; }

    bool lostPower() const { return lost_power_; }

    void setAlarm1(const DateTime& dt, uint8_t mode = 0) {
        alarm1_ = dt; alarm1_mode_ = mode;
        fprintf(stderr, "[esp32emu] DS3231: alarm1 set\n");
    }

    void setAlarm2(const DateTime& dt, uint8_t mode = 0) {
        alarm2_ = dt; alarm2_mode_ = mode;
        fprintf(stderr, "[esp32emu] DS3231: alarm2 set\n");
    }

    bool alarmFired(uint8_t alarm = 1) {
        if (alarm == 1) { bool f = alarm1_fired_; alarm1_fired_ = false; return f; }
        if (alarm == 2) { bool f = alarm2_fired_; alarm2_fired_ = false; return f; }
        return false;
    }

    void clearAlarm(uint8_t alarm = 1) {
        if (alarm == 1) alarm1_fired_ = false;
        if (alarm == 2) alarm2_fired_ = false;
    }

    void enable32K(bool en = true) { sq32k_ = en; }
    void writeSqwPinMode(uint8_t mode) { sqw_mode_ = mode; }

    // Test helpers
    void _setTemperature(float t) { temp_ = t; }
    void _setLostPower(bool v) { lost_power_ = v; }
    void _fireAlarm(uint8_t n) {
        if (n == 1) alarm1_fired_ = true;
        if (n == 2) alarm2_fired_ = true;
    }
    bool _isInitialized() const { return initialized_; }

private:
    bool initialized_ = false;
    DateTime now_;
    float temp_ = 25.0f;
    bool lost_power_ = false;
    DateTime alarm1_, alarm2_;
    uint8_t alarm1_mode_ = 0, alarm2_mode_ = 0;
    bool alarm1_fired_ = false, alarm2_fired_ = false;
    bool sq32k_ = false;
    uint8_t sqw_mode_ = 0;
};
