// U8g2 OLED display example
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C display;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    display.begin();
    display.setFont(u8g2_font_ncenB14_tr);
}

void loop() {
    display.clearBuffer();
    display.drawStr(10, 30, "ESP32emu");
    display.drawStr(10, 55, "U8g2 Demo");
    display.drawFrame(0, 0, 128, 64);
    display.drawCircle(100, 15, 10);
    display.sendBuffer();
    Serial.println("Display updated");
    delay(1000);
}
