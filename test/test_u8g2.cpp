// Test: U8g2 graphics library mock
#include "U8g2lib.h"
#include <cassert>
#include <cstdio>

int main() {
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C display;
    display.begin();

    assert(display.getDisplayWidth() == 128);
    assert(display.getDisplayHeight() == 64);

    // Draw text
    display.setFont(u8g2_font_ncenB08_tr);
    assert(display._getFont() == u8g2_font_ncenB08_tr);

    int w = display.drawStr(10, 20, "Hello");
    assert(w == 30);  // 5 chars * 6px
    assert(display._textCount() == 1);
    assert(display._textAt(0) == "Hello");
    assert(display._textX(0) == 10);
    assert(display._textY(0) == 20);

    // Draw shapes
    display.drawBox(0, 0, 32, 16);
    assert(display._boxCount() == 1);

    display.drawCircle(64, 32, 10);
    assert(display._circleCount() == 1);

    display.drawLine(0, 0, 127, 63);
    assert(display._lineCount() == 1);

    // Frame draws 4 lines
    display.drawFrame(10, 10, 50, 30);
    assert(display._lineCount() == 5);  // 1 + 4

    // Send buffer
    display.sendBuffer();
    assert(display._wasSent());

    // Pixel operations
    display.clearBuffer();
    display.drawPixel(5, 5);
    assert(display.getPixel(5, 5));
    assert(!display.getPixel(6, 5));

    // Box fills pixels
    display.clearBuffer();
    display.drawBox(10, 10, 3, 3);
    assert(display.getPixel(10, 10));
    assert(display.getPixel(12, 12));
    assert(!display.getPixel(13, 13));

    // String width
    assert(display.getStrWidth("test") == 24);

    // Power save
    display.setPowerSave(1);
    assert(!display._isPowered());
    display.setPowerSave(0);
    assert(display._isPowered());

    // Contrast
    display.setContrast(200);
    assert(display._getContrast() == 200);

    // I2C address
    display.setI2CAddress(0x3D);
    assert(display._getI2CAddr() == 0x3D);

    // Page mode
    display.firstPage();
    assert(!display.nextPage());

    // XBM
    uint8_t bitmap[] = {0xFF};
    display.drawXBM(0, 0, 8, 1, bitmap);
    assert(display._xbmWasDrawn());

    // Print
    display.setCursor(0, 50);
    display.print("Count:");
    display.print(42);

    // UTF8
    display.drawUTF8(0, 60, "Ünîcödé");

    printf("test_u8g2: all assertions passed\n");
    return 0;
}
