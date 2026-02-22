// esp32emu — DS3231 RTC test
#include "DS3231.h"
#include <cassert>
#include <cstdio>

int main() {
    DS3231 rtc;
    assert(!rtc._isInitialized());
    assert(rtc.begin());
    assert(rtc._isInitialized());

    // Default time
    DateTime dt = rtc.now();
    assert(dt.year == 2025);
    assert(dt.month == 1);
    assert(dt.day == 1);

    // Set time
    rtc.adjust(DateTime(2026, 2, 22, 4, 0, 0));
    dt = rtc.now();
    assert(dt.year == 2026);
    assert(dt.month == 2);
    assert(dt.day == 22);
    assert(dt.hour == 4);

    // Unixtime
    DateTime epoch(2000, 1, 1, 0, 0, 0);
    uint32_t ut = epoch.unixtime();
    assert(ut > 0);

    // Temperature
    assert(rtc.getTemperature() == 25.0f);
    rtc._setTemperature(23.5f);
    assert(rtc.getTemperature() == 23.5f);

    // Lost power
    assert(!rtc.lostPower());
    rtc._setLostPower(true);
    assert(rtc.lostPower());

    // Alarms
    rtc.setAlarm1(DateTime(2026, 2, 22, 5, 0, 0));
    rtc.setAlarm2(DateTime(2026, 2, 22, 6, 0, 0));
    assert(!rtc.alarmFired(1));
    assert(!rtc.alarmFired(2));

    rtc._fireAlarm(1);
    assert(rtc.alarmFired(1));  // reads and clears
    assert(!rtc.alarmFired(1));

    rtc._fireAlarm(2);
    rtc.clearAlarm(2);
    assert(!rtc.alarmFired(2));

    // 32K and SQW
    rtc.enable32K(true);
    rtc.writeSqwPinMode(1);

    printf("test_ds3231: all assertions passed\n");
    return 0;
}
