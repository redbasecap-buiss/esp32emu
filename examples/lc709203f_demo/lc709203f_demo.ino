// LC709203F battery fuel gauge demo
#include <Adafruit_LC709203F.h>

Adafruit_LC709203F gauge;

void setup() {
    Serial.begin(115200);
    if (!gauge.begin()) {
        Serial.println("LC709203F not found!");
        while (1) delay(10);
    }
    gauge.setPackSize(LC709203F_APA_2000MAH);
    gauge.setAlarmRSOC(10);
    Serial.print("IC Version: 0x");
    Serial.println(gauge.getICversion(), HEX);
    Serial.println("LC709203F ready!");
}

void loop() {
    Serial.print("Battery: ");
    Serial.print(gauge.cellVoltage(), 2);
    Serial.print("V  ");
    Serial.print(gauge.cellPercent(), 1);
    Serial.println("%");
    delay(5000);
}
