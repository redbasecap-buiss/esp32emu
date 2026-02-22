// test_st7789: ST7789 TFT display mock tests
#include "Adafruit_ST7789.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_ST7789 tft(5, 16, 17);

    // Init
    tft.init(240, 320);

    // Draw pixel and read back
    tft.drawPixel(10, 20, 0xF800); // red
    assert(tft.getPixel(10, 20) == 0xF800);
    assert(tft.getPixel(0, 0) == 0x0000);

    // Fill screen
    tft.fillScreen(0x001F); // blue
    assert(tft.getPixel(0, 0) == 0x001F);
    assert(tft.getPixel(100, 100) == 0x001F);

    // Fill rect
    tft.fillRect(10, 10, 5, 5, 0x07E0); // green
    assert(tft.getPixel(12, 12) == 0x07E0);
    assert(tft.getPixel(9, 9) == 0x001F); // outside rect

    // Rotation
    assert(tft.getRotation() == 0);
    tft.setRotation(2);
    assert(tft.getRotation() == 2);
    tft.setRotation(5); // wraps
    assert(tft.getRotation() == 1);

    // Invert
    assert(!tft.getInvert());
    tft.invertDisplay(true);
    assert(tft.getInvert());

    // Sleep
    assert(!tft.isSleeping());
    tft.enableSleep(true);
    assert(tft.isSleeping());
    tft.enableSleep(false);
    assert(!tft.isSleeping());

    // color565
    uint16_t white = Adafruit_ST7789::color565(255, 255, 255);
    assert(white == 0xFFFF);
    uint16_t black = Adafruit_ST7789::color565(0, 0, 0);
    assert(black == 0x0000);

    // SPI speed
    tft.setSPISpeed(80000000);
    assert(tft.getSPISpeed() == 80000000);

    // Out of bounds pixel
    tft.drawPixel(-1, -1, 0xFFFF); // should not crash
    tft.drawPixel(300, 400, 0xFFFF);
    assert(tft.getPixel(-1, -1) == 0); // returns 0 for OOB

    // HLine / VLine
    tft.fillScreen(0);
    tft.drawFastHLine(0, 0, 10, 0xAAAA);
    for (int i = 0; i < 10; i++) assert(tft.getPixel(i, 0) == 0xAAAA);
    assert(tft.getPixel(10, 0) == 0);

    tft.drawFastVLine(50, 0, 5, 0xBBBB);
    for (int i = 0; i < 5; i++) assert(tft.getPixel(50, i) == 0xBBBB);

    printf("test_st7789: all assertions passed\n");
    return 0;
}
