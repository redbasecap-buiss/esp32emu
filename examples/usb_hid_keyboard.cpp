// esp32emu example — ESP32-S3 USB HID Keyboard
#include "Arduino.h"
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard keyboard;
const int buttonPin = 0;

void setup() {
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP);

    USB.manufacturerName("esp32emu");
    USB.productName("Demo Keyboard");
    USB.begin();
    keyboard.begin();

    Serial.println("USB HID Keyboard Ready");

    // Demo: type "Hello World" when button pressed
    keyboard.print("Hello World");
    keyboard.write(KEY_RETURN);

    // Ctrl+C shortcut
    keyboard.press(KEY_LEFT_CTRL);
    keyboard.press('c');
    delay(50);
    keyboard.releaseAll();
}

void loop() {
    delay(100);
}
