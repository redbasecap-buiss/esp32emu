// SSD1306 OLED Dashboard
// Displays sensor data on a 128x64 OLED screen

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

int counter = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed!");
        return;
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 20);
    display.println("esp32emu");
    display.display();
    delay(1000);
}

void loop() {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("=== Dashboard ===");

    display.setCursor(0, 16);
    display.print("Uptime: ");
    display.print(millis() / 1000);
    display.println("s");

    display.setCursor(0, 28);
    display.print("Counter: ");
    display.println(counter++);

    display.setCursor(0, 40);
    display.print("Free RAM: ");
    display.print(ESP.getFreeHeap());
    display.println(" B");

    display.display();
    delay(1000);
}
