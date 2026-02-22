// esp32emu — test_ublox_gnss: SparkFun u-blox GNSS mock test
#include "SparkFun_u-blox_GNSS.h"
#include <cassert>
#include <cstdio>

int main() {
    SFE_UBLOX_GNSS gps;

    assert(!gps.test_isBegun());
    assert(gps.begin());
    assert(gps.test_isBegun());

    // Default position (Zurich-ish)
    assert(gps.getLatitude() == 471234567);
    assert(gps.getLongitude() == 85432100);
    assert(gps.getAltitude() == 450000);

    // Default fix
    assert(gps.getFixType() == 3); // 3D fix
    assert(gps.getSIV() == 8);
    assert(gps.getDateValid());
    assert(gps.getTimeValid());

    // Date/time
    assert(gps.getYear() == 2026);
    assert(gps.getMonth() == 2);
    assert(gps.getDay() == 22);

    // getPVT
    assert(gps.getPVT());

    // Set custom position
    gps.test_setPosition(408000000, -1222000000, 10000);
    assert(gps.getLatitude() == 408000000);    // San Francisco
    assert(gps.getLongitude() == -1222000000);
    assert(gps.getAltitude() == 10000);

    // Speed/heading
    gps.test_setSpeed(5000, 18000000); // 5 m/s, 180 degrees
    assert(gps.getGroundSpeed() == 5000);
    assert(gps.getHeading() == 18000000);

    // Satellites
    gps.test_setSIV(12);
    assert(gps.getSIV() == 12);

    // No fix
    gps.test_setFixType(0);
    assert(gps.getFixType() == 0);
    assert(!gps.getDateValid());
    assert(!gps.getTimeValid());

    // Custom date/time
    gps.test_setDateTime(2025, 12, 25, 8, 30, 45);
    assert(gps.getYear() == 2025);
    assert(gps.getHour() == 8);
    assert(gps.getSecond() == 45);

    // Config methods (no-op but shouldn't crash)
    gps.setI2COutput(0x01);
    gps.setNavigationFrequency(5);
    gps.setAutoPVT(true);
    gps.factoryDefault();

    printf("test_ublox_gnss: all assertions passed\n");
    return 0;
}
