// DS3231 RTC — Real-Time Clock Display
// Shows date/time and temperature from precision RTC module.

#include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>

DS3231 rtc;

const char* dayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!rtc.begin()) {
        Serial.println("DS3231 not found!");
        while (1) delay(100);
    }

    if (rtc.lostPower()) {
        Serial.println("⚠️ RTC lost power, setting time...");
        rtc.adjust(DateTime(2026, 2, 22, 4, 0, 0));
    }

    // Set alarm for 5 AM
    rtc.setAlarm1(DateTime(2026, 2, 22, 5, 0, 0));

    Serial.println("🕐 DS3231 RTC Clock Ready");
}

void loop() {
    DateTime now = rtc.now();

    char buf[64];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             now.year, now.month, now.day, now.hour, now.minute, now.second);
    Serial.print("📅 ");
    Serial.print(buf);
    Serial.print("  🌡️ ");
    Serial.print(rtc.getTemperature(), 1);
    Serial.println("°C");

    if (rtc.alarmFired(1)) {
        Serial.println("⏰ ALARM 1 fired!");
    }

    delay(1000);
}
