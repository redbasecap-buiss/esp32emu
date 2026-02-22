// MAX17048 LiPo fuel gauge demo
#include <MAX17048.h>

MAX17048 gauge;

void setup() {
    Serial.begin(115200);
    if (!gauge.begin()) {
        Serial.println("MAX17048 not found!");
        while (1) delay(10);
    }
    Serial.print("Version: 0x");
    Serial.println(gauge.getVersion(), HEX);
    gauge.setAlertThreshold(10);
    Serial.println("Alert threshold set to 10%");
}

void loop() {
    Serial.print("Voltage: ");
    Serial.print(gauge.cellVoltage(), 2);
    Serial.print("V  SOC: ");
    Serial.print(gauge.cellPercent(), 1);
    Serial.print("%  Rate: ");
    Serial.print(gauge.chargeRate(), 1);
    Serial.println("%/hr");

    if (gauge.isActiveAlert()) {
        Serial.println("⚠️ Low battery alert!");
        gauge.clearAlert();
    }
    delay(5000);
}
