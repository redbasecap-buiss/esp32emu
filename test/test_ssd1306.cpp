#include "Adafruit_SSD1306.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_SSD1306 display(128, 64);
    assert(!display.isBegun());
    assert(display.width() == 128);
    assert(display.height() == 64);

    assert(display.begin());
    assert(display.isBegun());

    // Print text
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println("Hello");
    display.print("Temp: ");
    display.print(22.5f);
    display.display();

    auto& lines = display.getPrintedLines();
    assert(lines.size() == 2);
    assert(lines[0] == "Hello");
    assert(lines[1] == "Temp: 22.50");
    assert(display.getDisplayCount() == 1);

    // Pixel drawing
    display.clearDisplay();
    display.drawPixel(10, 20, SSD1306_WHITE);
    assert(display.getPixel(10, 20) == 1);
    assert(display.getPixel(11, 20) == 0);

    // Invert pixel
    display.drawPixel(10, 20, SSD1306_INVERSE);
    assert(display.getPixel(10, 20) == 0);

    // Draw shapes
    display.drawRect(0, 0, 50, 30, SSD1306_WHITE);
    display.fillCircle(64, 32, 10, SSD1306_WHITE);
    display.drawLine(0, 0, 127, 63, SSD1306_WHITE);
    assert(display.getDrawCalls() == 3);

    // 128x32 variant
    Adafruit_SSD1306 small(128, 32);
    small.begin();
    assert(small.height() == 32);

    printf("test_ssd1306: all assertions passed\n");
    return 0;
}
