// ADS1115 16-bit ADC — read 4 channels and display voltages
#include <Arduino.h>
#include <Wire.h>
#include <ADS1115.h>

ADS1115 adc(0x48);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    adc.begin();
    adc.setGain(GAIN_ONE);  // +/- 4.096V range
    Serial.println("ADS1115 16-bit ADC ready (±4.096V range)");
}

void loop() {
    Serial.println("--- ADC Readings ---");
    for (int ch = 0; ch < 4; ch++) {
        int16_t raw = adc.readADC_SingleEnded(ch);
        float voltage = adc.computeVolts(raw);
        Serial.print("  CH");
        Serial.print(ch);
        Serial.print(": ");
        Serial.print(raw);
        Serial.print(" raw = ");
        Serial.print(voltage, 4);
        Serial.println("V");
    }

    // Differential reading
    int16_t diff01 = adc.readADC_Differential_0_1();
    Serial.print("  Diff 0-1: ");
    Serial.print(adc.computeVolts(diff01), 4);
    Serial.println("V");

    Serial.println();
    delay(1000);
}
