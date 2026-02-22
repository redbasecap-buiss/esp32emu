// esp32emu example — 4x4 Matrix Keypad
#include "Arduino.h"
#include "Keypad.h"

char keys[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
uint8_t rowPins[4] = {13, 12, 14, 27};
uint8_t colPins[4] = {26, 25, 33, 32};

Keypad keypad((char*)keys, rowPins, colPins, 4, 4);

void setup() {
    Serial.begin(115200);
    Serial.println("4x4 Keypad Ready");
    keypad.setHoldTime(500);
    keypad.setDebounceTime(50);

    // Simulate key presses for demo
    keypad.test_injectKey('5');
}

void loop() {
    char key = keypad.getKey();
    if (key != NO_KEY) {
        Serial.print("Key pressed: ");
        Serial.println(key);
    }
    delay(100);
}
