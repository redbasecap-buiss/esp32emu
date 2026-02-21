#include <SPI.h>
#include <Adafruit_MAX31865.h>

Adafruit_MAX31865 thermo(5); // CS on pin 5

#define RREF 430.0
#define RNOMINAL 100.0

void setup() {
    Serial.begin(115200);
    thermo.begin(Adafruit_MAX31865::MAX31865_3WIRE);
    Serial.println("MAX31865 PT100 RTD Sensor ready");
}

void loop() {
    uint16_t rtd = thermo.readRTD();
    float ratio = rtd / 32768.0;
    Serial.print("RTD: "); Serial.print(rtd);
    Serial.print("  Ratio: "); Serial.print(ratio, 4);
    Serial.print("  Resistance: "); Serial.print(RREF * ratio, 2);
    Serial.print("  Temp: "); Serial.print(thermo.temperature(RNOMINAL, RREF));
    Serial.println(" °C");

    uint8_t fault = thermo.readFault();
    if (fault) {
        Serial.print("Fault 0x"); Serial.println(fault, HEX);
        thermo.clearFault();
    }
    delay(1000);
}
