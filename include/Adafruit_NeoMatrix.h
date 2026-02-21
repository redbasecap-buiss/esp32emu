#pragma once
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "Adafruit_GFX.h"

// Matrix layout flags
#define NEO_MATRIX_TOP         0x00
#define NEO_MATRIX_BOTTOM      0x01
#define NEO_MATRIX_LEFT        0x00
#define NEO_MATRIX_RIGHT       0x02
#define NEO_MATRIX_CORNER      0x03
#define NEO_MATRIX_ROWS        0x00
#define NEO_MATRIX_COLUMNS     0x04
#define NEO_MATRIX_AXIS        0x04
#define NEO_MATRIX_PROGRESSIVE 0x00
#define NEO_MATRIX_ZIGZAG      0x08
#define NEO_MATRIX_SEQUENCE    0x08

// Tile layout flags
#define NEO_TILE_TOP           0x00
#define NEO_TILE_BOTTOM        0x10
#define NEO_TILE_LEFT          0x00
#define NEO_TILE_RIGHT         0x20
#define NEO_TILE_CORNER        0x30
#define NEO_TILE_ROWS          0x00
#define NEO_TILE_COLUMNS       0x40
#define NEO_TILE_AXIS          0x40
#define NEO_TILE_PROGRESSIVE   0x00
#define NEO_TILE_ZIGZAG        0x80
#define NEO_TILE_SEQUENCE      0x80

class Adafruit_NeoMatrix : public Adafruit_GFX {
public:
    // Single matrix constructor
    Adafruit_NeoMatrix(int w, int h, uint8_t pin,
                       uint8_t matrixType = NEO_MATRIX_TOP | NEO_MATRIX_LEFT | NEO_MATRIX_ROWS | NEO_MATRIX_PROGRESSIVE,
                       uint8_t ledType = NEO_GRB + NEO_KHZ800)
        : Adafruit_GFX(w, h), _matW(w), _matH(h), _tilesX(1), _tilesY(1),
          _pin(pin), _matrixType(matrixType), _ledType(ledType),
          _strip(w * h, pin, ledType) {}

    // Tiled matrix constructor
    Adafruit_NeoMatrix(int matW, int matH, int tilesX, int tilesY,
                       uint8_t pin,
                       uint8_t matrixType = NEO_MATRIX_TOP | NEO_MATRIX_LEFT | NEO_MATRIX_ROWS | NEO_MATRIX_PROGRESSIVE,
                       uint8_t ledType = NEO_GRB + NEO_KHZ800)
        : Adafruit_GFX(matW * tilesX, matH * tilesY),
          _matW(matW), _matH(matH), _tilesX(tilesX), _tilesY(tilesY),
          _pin(pin), _matrixType(matrixType), _ledType(ledType),
          _strip(matW * matH * tilesX * tilesY, pin, ledType) {}

    void begin() { _strip.begin(); }
    void show() { _strip.show(); }
    void setBrightness(uint8_t b) { _strip.setBrightness(b); }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        if (x < 0 || y < 0 || x >= width() || y >= height()) return;
        int idx = _pixelIndex(x, y);
        if (idx >= 0) {
            // Expand 565 color to 32-bit
            uint8_t r = ((color >> 11) & 0x1F) << 3;
            uint8_t g = ((color >> 5) & 0x3F) << 2;
            uint8_t b = (color & 0x1F) << 3;
            _strip.setPixelColor(idx, r, g, b);
        }
    }

    void fillScreen(uint16_t color) {
        uint8_t r = ((color >> 11) & 0x1F) << 3;
        uint8_t g = ((color >> 5) & 0x3F) << 2;
        uint8_t b = (color & 0x1F) << 3;
        for (uint16_t i = 0; i < _strip.numPixels(); i++) {
            _strip.setPixelColor(i, r, g, b);
        }
    }

    static uint16_t Color(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    uint32_t ColorHSV(uint16_t hue, uint8_t sat = 255, uint8_t val = 255) {
        return Adafruit_NeoPixel::ColorHSV(hue, sat, val);
    }

    Adafruit_NeoPixel& getStrip() { return _strip; }

    // ── Test helpers ──
    uint32_t _getPixelColor(uint16_t n) const { return _strip.getPixelColor(n); }
    int _getPixelIndex(int x, int y) const { return _pixelIndex(x, y); }

private:
    int _matW, _matH, _tilesX, _tilesY;
    uint8_t _pin, _matrixType, _ledType;
    Adafruit_NeoPixel _strip;

    int _pixelIndex(int x, int y) const {
        if (x < 0 || y < 0 || x >= _matW * _tilesX || y >= _matH * _tilesY) return -1;

        // Which tile?
        int tileX = x / _matW;
        int tileY = y / _matH;
        int localX = x % _matW;
        int localY = y % _matH;

        // Within-matrix mapping
        bool zigzag = (_matrixType & NEO_MATRIX_SEQUENCE);
        bool columns = (_matrixType & NEO_MATRIX_AXIS);
        bool bottomStart = (_matrixType & 0x01);
        bool rightStart = (_matrixType & 0x02);

        if (bottomStart) localY = _matH - 1 - localY;
        if (rightStart) localX = _matW - 1 - localX;

        int pixel;
        if (columns) {
            if (zigzag && (localX & 1)) localY = _matH - 1 - localY;
            pixel = localX * _matH + localY;
        } else {
            if (zigzag && (localY & 1)) localX = _matW - 1 - localX;
            pixel = localY * _matW + localX;
        }

        // Tile offset
        int tileOffset = (tileY * _tilesX + tileX) * (_matW * _matH);
        return tileOffset + pixel;
    }
};
