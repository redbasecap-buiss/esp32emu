#include <Wire.h>
#include <MAX30105.h>

MAX30105 particleSensor;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!particleSensor.begin()) {
        Serial.println("MAX30105 not found!");
        while (1);
    }

    particleSensor.setup(0x1F, 4, 2, 100, 411, 4096);
    Serial.println("Pulse oximeter ready");
}

void loop() {
    uint32_t irValue = particleSensor.getIR();
    uint32_t redValue = particleSensor.getRed();

    if (spo2_algorithm::checkForBeat(irValue)) {
        Serial.print("Beat! IR=");
        Serial.print(irValue);
        Serial.print(" Red=");
        Serial.println(redValue);
    }

    float temp = particleSensor.readTemperature();
    Serial.print("Sensor temp: ");
    Serial.print(temp);
    Serial.println(" °C");

    delay(100);
}
