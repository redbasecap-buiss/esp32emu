// esp32emu – U8g2 OLED/LCD graphics library mock
#pragma once
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>

// Font constants (just tags for testing)
#define u8g2_font_ncenB08_tr  1
#define u8g2_font_ncenB10_tr  2
#define u8g2_font_ncenB14_tr  3
#define u8g2_font_6x10_tf     4
#define u8g2_font_helvR08_tr  5
#define u8g2_font_helvR14_tr  6

class U8G2 {
public:
    U8G2() : _width(128), _height(64), _font(0), _drawColor(1),
             _cursorX(0), _cursorY(0), _rotation(0), _power(true) {
        _buffer.resize((_width * _height + 7) / 8, 0);
    }

    void begin() { clearBuffer(); _power = true; }

    // Buffer ops
    void clearBuffer() { std::fill(_buffer.begin(), _buffer.end(), 0); }
    void sendBuffer() { _sent = true; }
    void clearDisplay() { clearBuffer(); sendBuffer(); }

    // Page mode
    void firstPage() { clearBuffer(); _page = 0; }
    bool nextPage() { _page++; return _page < 1; }

    // Drawing
    void drawPixel(int x, int y) { setPixel(x, y, _drawColor); }

    void drawLine(int x0, int y0, int x1, int y1) {
        _lines.push_back({x0, y0, x1, y1});
    }

    void drawHLine(int x, int y, int w) { drawLine(x, y, x + w - 1, y); }
    void drawVLine(int x, int y, int h) { drawLine(x, y, x, y + h - 1); }

    void drawFrame(int x, int y, int w, int h) {
        drawHLine(x, y, w);
        drawHLine(x, y + h - 1, w);
        drawVLine(x, y, h);
        drawVLine(x + w - 1, y, h);
    }

    void drawBox(int x, int y, int w, int h) {
        for (int j = y; j < y + h; j++)
            for (int i = x; i < x + w; i++)
                setPixel(i, j, _drawColor);
        _boxes.push_back({x, y, w, h});
    }

    void drawRFrame(int x, int y, int w, int h, int r) {
        (void)r; drawFrame(x, y, w, h);
    }
    void drawRBox(int x, int y, int w, int h, int r) {
        (void)r; drawBox(x, y, w, h);
    }

    void drawCircle(int x, int y, int r, uint8_t opt = 0xF) {
        (void)opt; _circles.push_back({x, y, r});
    }
    void drawDisc(int x, int y, int r, uint8_t opt = 0xF) {
        (void)opt; _circles.push_back({x, y, r});
    }

    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
        drawLine(x0, y0, x1, y1);
        drawLine(x1, y1, x2, y2);
        drawLine(x2, y2, x0, y0);
    }

    void drawEllipse(int x, int y, int rx, int ry, uint8_t opt = 0xF) {
        (void)opt; (void)x; (void)y; (void)rx; (void)ry;
    }

    // XBM/bitmap
    void drawXBM(int x, int y, int w, int h, const uint8_t* bitmap) {
        (void)x; (void)y; (void)w; (void)h; (void)bitmap;
        _xbmDrawn = true;
    }
    void drawXBMP(int x, int y, int w, int h, const uint8_t* bitmap) {
        drawXBM(x, y, w, h, bitmap);
    }

    // Text
    void setFont(uint8_t font) { _font = font; }
    void setFontMode(uint8_t mode) { _fontMode = mode; }
    void setDrawColor(uint8_t color) { _drawColor = color; }

    int drawStr(int x, int y, const char* str) {
        _texts.push_back({x, y, str ? str : ""});
        return str ? (int)strlen(str) * 6 : 0;
    }

    void drawUTF8(int x, int y, const char* str) { drawStr(x, y, str); }

    void setCursor(int x, int y) { _cursorX = x; _cursorY = y; }
    void print(const char* str) {
        drawStr(_cursorX, _cursorY, str);
        _cursorX += str ? (int)strlen(str) * 6 : 0;
    }
    void print(int val) { print(std::to_string(val).c_str()); }
    void print(float val) { print(std::to_string(val).c_str()); }

    int getStrWidth(const char* str) const { return str ? (int)strlen(str) * 6 : 0; }
    int getAscent() const { return 8; }
    int getDescent() const { return -2; }
    int getMaxCharWidth() const { return 6; }
    int getMaxCharHeight() const { return 10; }

    // Display info
    int getDisplayWidth() const { return _width; }
    int getDisplayHeight() const { return _height; }
    uint8_t* getBufferPtr() { return _buffer.data(); }
    int getBufferTileHeight() const { return _height / 8; }
    int getBufferTileWidth() const { return _width / 8; }

    // Power
    void setPowerSave(uint8_t save) { _power = !save; }
    void setContrast(uint8_t contrast) { _contrast = contrast; }
    void setFlipMode(uint8_t mode) { _rotation = mode; }

    // I2C address
    void setI2CAddress(uint8_t addr) { _i2cAddr = addr; }

    // Test helpers
    bool _wasSent() const { return _sent; }
    size_t _textCount() const { return _texts.size(); }
    std::string _textAt(size_t i) const { return i < _texts.size() ? _texts[i].text : ""; }
    int _textX(size_t i) const { return i < _texts.size() ? _texts[i].x : -1; }
    int _textY(size_t i) const { return i < _texts.size() ? _texts[i].y : -1; }
    size_t _boxCount() const { return _boxes.size(); }
    size_t _circleCount() const { return _circles.size(); }
    size_t _lineCount() const { return _lines.size(); }
    bool _xbmWasDrawn() const { return _xbmDrawn; }
    uint8_t _getFont() const { return _font; }
    bool _isPowered() const { return _power; }
    uint8_t _getContrast() const { return _contrast; }
    uint8_t _getI2CAddr() const { return _i2cAddr; }

    bool getPixel(int x, int y) const {
        if (x < 0 || x >= _width || y < 0 || y >= _height) return false;
        int idx = y * _width + x;
        return (_buffer[idx / 8] >> (idx % 8)) & 1;
    }

private:
    void setPixel(int x, int y, uint8_t color) {
        if (x < 0 || x >= _width || y < 0 || y >= _height) return;
        int idx = y * _width + x;
        if (color) _buffer[idx / 8] |= (1 << (idx % 8));
        else _buffer[idx / 8] &= ~(1 << (idx % 8));
    }

    int _width, _height;
    uint8_t _font = 0;
    uint8_t _fontMode = 0;
    uint8_t _drawColor;
    int _cursorX, _cursorY;
    uint8_t _rotation;
    bool _power;
    bool _sent = false;
    int _page = 0;
    uint8_t _contrast = 128;
    uint8_t _i2cAddr = 0x3C;
    bool _xbmDrawn = false;
    std::vector<uint8_t> _buffer;

    struct TextEntry { int x, y; std::string text; };
    struct Box { int x, y, w, h; };
    struct Circle { int x, y, r; };
    struct Line { int x0, y0, x1, y1; };

    std::vector<TextEntry> _texts;
    std::vector<Box> _boxes;
    std::vector<Circle> _circles;
    std::vector<Line> _lines;
};

// Common display constructors (all map to same mock)
using U8G2_SSD1306_128X64_NONAME_F_HW_I2C = U8G2;
using U8G2_SSD1306_128X64_NONAME_F_SW_I2C = U8G2;
using U8G2_SSD1306_128X64_NONAME_1_HW_I2C = U8G2;
using U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C = U8G2;
using U8G2_SH1106_128X64_NONAME_F_HW_I2C = U8G2;
using U8G2_ST7920_128X64_F_SW_SPI = U8G2;
using U8G2_MAX7219_32X8_F_4W_SW_SPI = U8G2;
using U8G2_SSD1309_128X64_NONAME0_F_HW_I2C = U8G2;
