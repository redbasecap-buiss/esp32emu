// TCS34725 color sensor demo
#include <Adafruit_TCS34725.h>

Adafruit_TCS34725 tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
    Serial.begin(115200);
    if (!tcs.begin()) {
        Serial.println("TCS34725 not found!");
        while (1) delay(10);
    }
    Serial.println("TCS34725 color sensor ready!");
}

void loop() {
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);

    uint16_t colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
    uint16_t lux = tcs.calculateLux(r, g, b);

    Serial.print("R: "); Serial.print(r);
    Serial.print("  G: "); Serial.print(g);
    Serial.print("  B: "); Serial.print(b);
    Serial.print("  C: "); Serial.println(c);
    Serial.print("Color Temp: "); Serial.print(colorTemp);
    Serial.print(" K  Lux: "); Serial.println(lux);
    Serial.println("---");
    delay(1000);
}
