#include "Adafruit_GFX.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test GFXcanvas16 (concrete subclass)
    GFXcanvas16 canvas(128, 64);
    assert(canvas.width() == 128);
    assert(canvas.height() == 64);

    // Draw pixel
    canvas.drawPixel(10, 20, GFX_WHITE);
    assert(canvas.getPixel(10, 20) == GFX_WHITE);
    assert(canvas.getPixelCount() == 1);

    // Out of bounds pixel
    canvas.drawPixel(-1, 0, GFX_RED);
    canvas.drawPixel(200, 0, GFX_RED);
    assert(canvas.getPixel(0, 0) == 0); // not written

    // Fill rect
    canvas.fillRect(0, 0, 10, 10, GFX_RED);
    assert(canvas.getFillRectCount() == 1);

    // Fill screen
    canvas.fillScreen(GFX_BLACK);
    assert(canvas.getFillRectCount() == 2);

    // Lines and circles
    canvas.drawLine(0, 0, 127, 63, GFX_GREEN);
    assert(canvas.getLineCount() == 1);
    canvas.drawCircle(64, 32, 10, GFX_BLUE);
    assert(canvas.getCircleCount() == 1);

    // Text
    canvas.setCursor(0, 0);
    assert(canvas.getCursorX() == 0);
    canvas.setTextColor(GFX_WHITE);
    canvas.setTextSize(1);
    canvas.print("Hello");
    assert(canvas.getPrintedText() == "Hello");

    canvas.println(" World");
    assert(canvas.getPrintedText().find("World") != std::string::npos);

    // Rotation
    canvas.setRotation(1);
    assert(canvas.width() == 64);
    assert(canvas.height() == 128);
    assert(canvas.getRotation() == 1);

    canvas.setRotation(0);
    assert(canvas.width() == 128);

    // color565
    uint16_t white = Adafruit_GFX::color565(255, 255, 255);
    assert(white == 0xFFFF);
    uint16_t black = Adafruit_GFX::color565(0, 0, 0);
    assert(black == 0x0000);

    // Text bounds
    int16_t x1, y1; uint16_t w, h;
    canvas.setTextSize(2);
    canvas.getTextBounds("Hi", 0, 0, &x1, &y1, &w, &h);
    assert(w == 2 * 6 * 2);  // 2 chars * 6px * size 2
    assert(h == 8 * 2);

    // Reset counters
    canvas.resetCounters();
    assert(canvas.getPixelCount() == 0);
    assert(canvas.getPrintedText().empty());

    // Buffer access
    assert(canvas.getBuffer() != nullptr);

    printf("test_adafruit_gfx: all assertions passed\n");
    return 0;
}
