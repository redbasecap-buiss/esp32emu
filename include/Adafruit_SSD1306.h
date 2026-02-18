#pragma once
// esp32emu: Adafruit SSD1306 OLED display mock (128x64, 128x32)
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

#define SSD1306_SWITCHCAPVCC 0x02
#define SSD1306_EXTERNALVCC  0x01

#define SSD1306_BLACK   0
#define SSD1306_WHITE   1
#define SSD1306_INVERSE 2

class Adafruit_SSD1306 {
public:
    Adafruit_SSD1306(int16_t w, int16_t h, void* wire = nullptr, int8_t rst = -1)
        : _width(w), _height(h), _cursorX(0), _cursorY(0), _textSize(1),
          _textColor(SSD1306_WHITE), _begun(false) {
        (void)wire; (void)rst;
        _buffer.resize((w * h) / 8, 0);
    }

    bool begin(uint8_t vcs = SSD1306_SWITCHCAPVCC, uint8_t addr = 0x3C) {
        (void)vcs; (void)addr;
        _begun = true;
        clearDisplay();
        return true;
    }

    void clearDisplay() {
        std::fill(_buffer.begin(), _buffer.end(), 0);
        _printedLines.clear();
        _currentLine.clear();
    }

    void display() {
        if (!_currentLine.empty()) {
            _printedLines.push_back(_currentLine);
            _currentLine.clear();
        }
        _displayCount++;
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        if (x < 0 || x >= _width || y < 0 || y >= _height) return;
        size_t idx = x + (y / 8) * _width;
        if (idx >= _buffer.size()) return;
        if (color == SSD1306_WHITE)
            _buffer[idx] |= (1 << (y & 7));
        else if (color == SSD1306_BLACK)
            _buffer[idx] &= ~(1 << (y & 7));
        else
            _buffer[idx] ^= (1 << (y & 7));
    }

    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
        (void)x0; (void)y0; (void)x1; (void)y1; (void)color;
        _drawCalls++;
    }

    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        (void)x; (void)y; (void)w; (void)h; (void)color;
        _drawCalls++;
    }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        (void)x; (void)y; (void)w; (void)h; (void)color;
        _drawCalls++;
    }

    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
        (void)x; (void)y; (void)r; (void)color;
        _drawCalls++;
    }

    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
        (void)x; (void)y; (void)r; (void)color;
        _drawCalls++;
    }

    void drawBitmap(int16_t x, int16_t y, const uint8_t* bmp, int16_t w, int16_t h, uint16_t color) {
        (void)x; (void)y; (void)bmp; (void)w; (void)h; (void)color;
        _drawCalls++;
    }

    void setCursor(int16_t x, int16_t y) { _cursorX = x; _cursorY = y; }
    void setTextSize(uint8_t s) { _textSize = s; }
    void setTextColor(uint16_t c) { _textColor = c; }
    void setTextColor(uint16_t c, uint16_t bg) { _textColor = c; (void)bg; }

    void print(const char* s) { _currentLine += s; }
    void print(int v) { _currentLine += std::to_string(v); }
    void print(float v, int dec = 2) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.*f", dec, v);
        _currentLine += buf;
    }
    void println(const char* s = "") {
        _currentLine += s;
        _printedLines.push_back(_currentLine);
        _currentLine.clear();
    }
    void println(int v) { print(v); println(); }
    void println(float v, int dec = 2) { print(v, dec); println(); }

    void invertDisplay(bool i) { _inverted = i; }
    void dim(bool d) { _dimmed = d; }

    int16_t width() const { return _width; }
    int16_t height() const { return _height; }

    // Test helpers
    const std::vector<std::string>& getPrintedLines() const { return _printedLines; }
    int getDisplayCount() const { return _displayCount; }
    int getDrawCalls() const { return _drawCalls; }
    bool isBegun() const { return _begun; }
    uint8_t getPixel(int16_t x, int16_t y) const {
        if (x < 0 || x >= _width || y < 0 || y >= _height) return 0;
        size_t idx = x + (y / 8) * _width;
        if (idx >= _buffer.size()) return 0;
        return (_buffer[idx] >> (y & 7)) & 1;
    }

private:
    int16_t _width, _height;
    int16_t _cursorX, _cursorY;
    uint8_t _textSize;
    uint16_t _textColor;
    bool _begun;
    bool _inverted = false;
    bool _dimmed = false;
    int _displayCount = 0;
    int _drawCalls = 0;
    std::vector<uint8_t> _buffer;
    std::vector<std::string> _printedLines;
    std::string _currentLine;
};
