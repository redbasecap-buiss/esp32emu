// Modbus RTU — Read temperature sensor via RS-485
#include <Arduino.h>
#include <ModbusMaster.h>

#define DE_PIN 4
#define RE_PIN 5

ModbusMaster node;

void preTransmission() {
    digitalWrite(DE_PIN, HIGH);
    digitalWrite(RE_PIN, HIGH);
}

void postTransmission() {
    digitalWrite(DE_PIN, LOW);
    digitalWrite(RE_PIN, LOW);
}

void setup() {
    Serial.begin(115200);
    pinMode(DE_PIN, OUTPUT);
    pinMode(RE_PIN, OUTPUT);
    digitalWrite(DE_PIN, LOW);
    digitalWrite(RE_PIN, LOW);

    Serial2.begin(9600);
    node.begin(1, Serial2);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);

    // Test: pre-set register with 25.5°C (255 = 25.5 * 10)
    node.emu_setRegister(0x0001, 255);

    Serial.println("Modbus Temperature Reader Ready");
}

void loop() {
    uint8_t result = node.readHoldingRegisters(0x0001, 1);
    if (result == ModbusMaster::ku8MBSuccess) {
        float temp = node.getResponseBuffer(0) / 10.0f;
        Serial.print("Temperature: ");
        Serial.print(temp, 1);
        Serial.println(" °C");
    } else {
        Serial.println("Modbus read failed!");
    }
    delay(2000);
}
