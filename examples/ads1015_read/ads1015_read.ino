#include <Wire.h>
#include <ADS1015.h>

Adafruit_ADS1015 ads;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    ads.begin(0x48);
    ads.setGain(Adafruit_ADS1015::GAIN_ONE);
    Serial.println("ADS1015 4-channel ADC");
}

void loop() {
    for (int ch = 0; ch < 4; ch++) {
        int16_t raw = ads.readADC_SingleEnded(ch);
        float volts = ads.computeVolts(raw);
        Serial.print("CH");
        Serial.print(ch);
        Serial.print(": ");
        Serial.print(raw);
        Serial.print(" (");
        Serial.print(volts);
        Serial.println(" V)");
    }
    Serial.println("---");
    delay(1000);
}
