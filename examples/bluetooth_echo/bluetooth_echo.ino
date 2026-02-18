// Bluetooth Echo — Receives data via BT Serial and echoes it back
#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() {
    Serial.begin(115200);
    SerialBT.begin("ESP32-Echo");
    Serial.println("Bluetooth Echo Server started");
    Serial.println("Device name: ESP32-Echo");
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (SerialBT.available()) {
        String received = SerialBT.readStringUntil('\n');
        Serial.print("Received: ");
        Serial.println(received.c_str());

        // Echo back with prefix
        SerialBT.print("Echo: ");
        SerialBT.println(received.c_str());

        // Blink LED on receive
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
    }
    delay(20);
}
