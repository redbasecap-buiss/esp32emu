#include <Wire.h>
#include <Adafruit_VEML6075.h>

Adafruit_VEML6075 uv;

void setup() {
    Serial.begin(115200);
    if (!uv.begin()) {
        Serial.println("VEML6075 not found!");
        while (1) delay(10);
    }
    uv.setIntegrationTime(Adafruit_VEML6075::VEML6075_100MS);
    uv.setHighDynamic(true);
    Serial.println("VEML6075 UV Sensor ready");
}

void loop() {
    Serial.print("UVA: "); Serial.print(uv.readUVA());
    Serial.print("  UVB: "); Serial.print(uv.readUVB());
    Serial.print("  UV Index: "); Serial.println(uv.readUVI());
    delay(1000);
}
