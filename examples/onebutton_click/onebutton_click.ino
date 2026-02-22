// OneButton — Click, Double-Click, Long Press
// Demonstrates single button with multiple actions.

#include <Arduino.h>
#include <OneButton.h>

#define BUTTON_PIN 4
#define LED_PIN 2

OneButton btn(BUTTON_PIN, true, true);
bool ledState = false;

void handleClick() {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    Serial.println("💡 Click — LED toggled");
}

void handleDoubleClick() {
    Serial.println("⚡ Double-click — Fast blink!");
    for (int i = 0; i < 6; i++) {
        digitalWrite(LED_PIN, i % 2);
        delay(100);
    }
}

void handleLongPress() {
    Serial.println("🔴 Long press — System reset");
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    btn.attachClick(handleClick);
    btn.attachDoubleClick(handleDoubleClick);
    btn.attachLongPressStart(handleLongPress);

    Serial.println("OneButton Demo Ready");
    Serial.println("  Click = toggle LED");
    Serial.println("  Double-click = fast blink");
    Serial.println("  Long press = reset");
}

void loop() {
    btn.tick();
    delay(10);
}
