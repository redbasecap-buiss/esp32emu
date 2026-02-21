#include "Adafruit_NeoMatrix.h"
#include <cassert>
#include <cstdio>

int main() {
    printf("test_neomatrix: ");

    // 8x8 single matrix
    Adafruit_NeoMatrix matrix(8, 8, 6);
    matrix.begin();
    matrix.setBrightness(50);

    // Draw pixel
    uint16_t red = Adafruit_NeoMatrix::Color(255, 0, 0);
    matrix.drawPixel(0, 0, red);
    matrix.show();

    // Check pixel index mapping
    int idx = matrix._getPixelIndex(0, 0);
    assert(idx == 0);
    idx = matrix._getPixelIndex(1, 0);
    assert(idx == 1);
    idx = matrix._getPixelIndex(0, 1);
    assert(idx == 8);

    // Out of bounds returns -1
    idx = matrix._getPixelIndex(-1, 0);
    assert(idx == -1);
    idx = matrix._getPixelIndex(8, 0);
    assert(idx == -1);

    // Fill screen
    uint16_t blue = Adafruit_NeoMatrix::Color(0, 0, 255);
    matrix.fillScreen(blue);
    matrix.show();

    // Color conversion
    uint16_t green = Adafruit_NeoMatrix::Color(0, 255, 0);
    assert(green != 0);
    assert(red != green);
    assert(red != blue);

    // Tiled matrix (2x2 tiles of 4x4)
    Adafruit_NeoMatrix tiled(4, 4, 2, 2, 6);
    tiled.begin();
    assert(tiled.width() == 8);
    assert(tiled.height() == 8);

    // Pixel in second tile
    idx = tiled._getPixelIndex(4, 0);
    assert(idx >= 16); // should be in tile 1

    // Draw on tiled without crash
    tiled.drawPixel(7, 7, red);
    tiled.show();

    printf("all assertions passed\n");
    return 0;
}
