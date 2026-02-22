// esp32emu example — SparkFun u-blox GNSS Position
#include "Arduino.h"
#include "SparkFun_u-blox_GNSS.h"

SFE_UBLOX_GNSS gps;

void setup() {
    Serial.begin(115200);
    Serial.println("u-blox GNSS Demo");

    if (!gps.begin()) {
        Serial.println("GNSS not found!");
        while (1) delay(10);
    }

    gps.setNavigationFrequency(5);
    gps.setAutoPVT(true);

    // Simulate a walk through Zurich
    gps.test_setPosition(473769000, 85419000, 408000);
    gps.test_setSIV(10);
    gps.test_setSpeed(1400, 4500000); // 1.4 m/s, 45 degrees NE
}

void loop() {
    if (gps.getPVT()) {
        float lat = gps.getLatitude() / 1e7;
        float lon = gps.getLongitude() / 1e7;
        float alt = gps.getAltitude() / 1000.0;

        Serial.print("Lat: "); Serial.print(lat, 7);
        Serial.print(" Lon: "); Serial.print(lon, 7);
        Serial.print(" Alt: "); Serial.print(alt, 1); Serial.println("m");
        Serial.print("Satellites: "); Serial.println(gps.getSIV());
        Serial.print("Fix: "); Serial.println(gps.getFixType());
        Serial.print("Speed: "); Serial.print(gps.getGroundSpeed() / 1000.0);
        Serial.println(" m/s");
    }
    delay(200);
}
