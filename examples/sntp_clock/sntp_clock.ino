// SNTP time sync example — get current time from NTP
#include <Arduino.h>
#include "esp_sntp.h"
#include <ctime>

void time_sync_cb(struct timeval* tv) {
    Serial.println("⏰ Time synchronized!");
}

void setup() {
    Serial.begin(115200);
    Serial.println("SNTP Clock Example");

    // Configure SNTP
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "time.google.com");
    esp32emu_sntp_set_sync_callback(time_sync_cb);

    // Start
    sntp_init();

    // Set timezone (Central European Time)
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
}

void loop() {
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.print("Current time: ");
    Serial.println(buf);

    delay(1000);
}
