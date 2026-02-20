#include "TFT_eSPI.h"
#include <cassert>
#include <cstdio>

int main() {
    TFT_eSPI tft;
    assert(!tft.isInited());
    tft.init();
    assert(tft.isInited());

    // Default size 240x320
    assert(tft.width() == 240);
    assert(tft.height() == 320);

    // Rotation
    tft.setRotation(1);
    assert(tft.width() == 320);
    assert(tft.height() == 240);
    assert(tft.getRotation() == 1);
    tft.setRotation(0);

    // Drawing
    tft.fillScreen(TFT_BLACK);
    assert(tft.getFillCount() == 1);

    tft.drawRect(10, 10, 50, 30, TFT_WHITE);
    tft.drawCircle(100, 100, 20, TFT_RED);
    tft.drawLine(0, 0, 239, 319, TFT_GREEN);
    assert(tft.getDrawCount() == 3);

    tft.fillRect(10, 10, 50, 30, TFT_BLUE);
    tft.fillCircle(100, 100, 15, TFT_YELLOW);
    assert(tft.getFillCount() == 3);

    // Text
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    assert(tft.getTextDatum() == MC_DATUM);

    tft.drawString("Hello", 120, 160);
    assert(tft.getTextCount() == 1);
    assert(tft.getPrintedText() == "Hello");

    tft.drawNumber(42, 120, 180);
    assert(tft.getPrintedText().find("42") != std::string::npos);

    tft.drawFloat(3.14f, 2, 120, 200);
    assert(tft.getPrintedText().find("3.14") != std::string::npos);

    // Text width/height
    int16_t tw = tft.textWidth("Test");
    assert(tw == 4 * 6 * 2);  // 4 chars * 6px * size 2
    assert(tft.fontHeight() == 8 * 2);

    // Cursor
    tft.setCursor(50, 100);
    assert(tft.getCursorX() == 50);
    assert(tft.getCursorY() == 100);

    // Color conversion
    assert(TFT_eSPI::color565(255, 0, 0) == TFT_RED);
    assert(TFT_eSPI::color565(0, 0, 255) == TFT_BLUE);

    // Invert
    tft.invertDisplay(true);
    assert(tft.isInverted());

    // Print
    tft.resetCounters();
    tft.print("abc");
    tft.println("def");
    assert(tft.getPrintedText() == "abcdef\n");

    // Reset
    tft.resetCounters();
    assert(tft.getDrawCount() == 0);
    assert(tft.getFillCount() == 0);
    assert(tft.getTextCount() == 0);
    assert(tft.getPrintedText().empty());

    // Sprite
    TFT_eSprite sprite(&tft);
    assert(!sprite.isCreated());
    sprite.createSprite(64, 32);
    assert(sprite.isCreated());
    assert(sprite.spriteWidth() == 64);
    assert(sprite.spriteHeight() == 32);

    sprite.fillScreen(TFT_RED);
    sprite.drawString("Spr", 0, 0);
    sprite.pushSprite(10, 10);

    sprite.deleteSprite();
    assert(!sprite.isCreated());

    // begin() alias
    TFT_eSPI tft2(320, 240);
    tft2.begin();
    assert(tft2.isInited());
    assert(tft2.width() == 320);

    printf("test_tft_espi: all assertions passed\n");
    return 0;
}
