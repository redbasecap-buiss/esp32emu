// Adafruit_GFX canvas drawing example
#include "Arduino.h"
#include "Adafruit_GFX.h"
#include <cstdio>

GFXcanvas16 canvas(128, 64);

void setup() {
    Serial.begin(115200);

    canvas.fillScreen(GFX_BLACK);
    canvas.setTextColor(GFX_WHITE);
    canvas.setTextSize(1);

    // Draw border
    canvas.drawRect(0, 0, 128, 64, GFX_WHITE);

    // Draw some shapes
    canvas.fillCircle(32, 32, 10, GFX_RED);
    canvas.drawLine(0, 0, 127, 63, GFX_GREEN);

    // Text
    canvas.setCursor(45, 28);
    canvas.print("esp32emu");

    printf("Canvas: %dx%d, %u pixels drawn, %u fills, text: '%s'\n",
           canvas.width(), canvas.height(),
           canvas.getPixelCount(), canvas.getFillRectCount(),
           canvas.getPrintedText().c_str());

    // Color test
    uint16_t red = Adafruit_GFX::color565(255, 0, 0);
    printf("Red in RGB565: 0x%04X (expected 0xF800)\n", red);
}

void loop() {
    delay(1000);
}
