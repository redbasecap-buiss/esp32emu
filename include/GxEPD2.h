// esp32emu — GxEPD2 mock: E-paper display library
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <algorithm>

// Common display sizes
#define GxEPD2_154     1  // 200x200
#define GxEPD2_213     2  // 250x122
#define GxEPD2_290     3  // 296x128
#define GxEPD2_420     4  // 400x300
#define GxEPD2_750     5  // 800x480

// Colors
#define GxEPD_BLACK    0x0000
#define GxEPD_WHITE    0xFFFF
#define GxEPD_RED      0xF800

class GxEPD2_BW_Base {
public:
    GxEPD2_BW_Base(int16_t w, int16_t h)
        : _width(w), _height(h), _rotation(0), _initialized(false),
          _partialUpdates(0), _fullUpdates(0), _cursorX(0), _cursorY(0),
          _textColor(GxEPD_BLACK), _textSize(1) {
        size_t sz = (size_t)(w * h / 8 + 1);
        _buffer = new uint8_t[sz]();
        _bufSize = sz;
    }

    virtual ~GxEPD2_BW_Base() { delete[] _buffer; }

    void init(uint32_t baud = 115200) {
        (void)baud;
        _initialized = true;
        printf("[esp32emu] GxEPD2: %dx%d e-paper initialized\n", _width, _height);
    }

    void setRotation(uint8_t r) { _rotation = r % 4; }
    uint8_t getRotation() const { return _rotation; }
    int16_t width() const { return (_rotation & 1) ? _height : _width; }
    int16_t height() const { return (_rotation & 1) ? _width : _height; }

    void fillScreen(uint16_t color) {
        memset(_buffer, color == GxEPD_BLACK ? 0x00 : 0xFF, _bufSize);
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        if (x < 0 || x >= width() || y < 0 || y >= height()) return;
        size_t idx = (y * width() + x) / 8;
        uint8_t bit = 7 - (x % 8);
        if (idx < _bufSize) {
            if (color == GxEPD_BLACK) _buffer[idx] &= ~(1 << bit);
            else _buffer[idx] |= (1 << bit);
        }
    }

    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        for (int16_t i = x; i < x + w; i++) { drawPixel(i, y, color); drawPixel(i, y + h - 1, color); }
        for (int16_t j = y; j < y + h; j++) { drawPixel(x, j, color); drawPixel(x + w - 1, j, color); }
    }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        for (int16_t j = y; j < y + h; j++)
            for (int16_t i = x; i < x + w; i++)
                drawPixel(i, j, color);
    }

    void setCursor(int16_t x, int16_t y) { _cursorX = x; _cursorY = y; }
    void setTextColor(uint16_t c) { _textColor = c; }
    void setTextSize(uint8_t s) { _textSize = s > 0 ? s : 1; }

    void print(const char* text) {
        if (text) printf("[esp32emu] GxEPD2: print \"%s\" at (%d,%d)\n", text, _cursorX, _cursorY);
    }
    void println(const char* text = "") { print(text); _cursorY += 8 * _textSize; }

    void display(bool partial = false) {
        if (partial) { _partialUpdates++; }
        else { _fullUpdates++; }
    }

    void setPartialWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
        (void)x; (void)y; (void)w; (void)h;
    }

    void setFullWindow() {}

    void firstPage() { _paging = true; }
    bool nextPage() { _paging = false; display(); return false; }

    void hibernate() {
        printf("[esp32emu] GxEPD2: hibernate\n");
        _hibernating = true;
    }

    void powerOff() {
        printf("[esp32emu] GxEPD2: power off\n");
    }

    // --- Test helpers ---
    bool test_isInitialized() const { return _initialized; }
    int test_getFullUpdates() const { return _fullUpdates; }
    int test_getPartialUpdates() const { return _partialUpdates; }
    uint8_t test_getPixel(int16_t x, int16_t y) const {
        if (x < 0 || x >= width() || y < 0 || y >= height()) return 0;
        size_t idx = (y * width() + x) / 8;
        uint8_t bit = 7 - (x % 8);
        return (idx < _bufSize) ? ((_buffer[idx] >> bit) & 1) : 0;
    }
    bool test_isHibernating() const { return _hibernating; }

protected:
    int16_t _width, _height;
    uint8_t _rotation;
    bool _initialized;
    uint8_t* _buffer;
    size_t _bufSize;
    int _partialUpdates, _fullUpdates;
    int16_t _cursorX, _cursorY;
    uint16_t _textColor;
    uint8_t _textSize;
    bool _paging = false;
    bool _hibernating = false;
};

// Concrete display types
class GxEPD2_154_D67 : public GxEPD2_BW_Base {
public: GxEPD2_154_D67(int8_t cs = -1, int8_t dc = -1, int8_t rst = -1, int8_t busy = -1)
    : GxEPD2_BW_Base(200, 200) { (void)cs; (void)dc; (void)rst; (void)busy; }
};

class GxEPD2_213_B74 : public GxEPD2_BW_Base {
public: GxEPD2_213_B74(int8_t cs = -1, int8_t dc = -1, int8_t rst = -1, int8_t busy = -1)
    : GxEPD2_BW_Base(250, 122) { (void)cs; (void)dc; (void)rst; (void)busy; }
};

class GxEPD2_290_T94 : public GxEPD2_BW_Base {
public: GxEPD2_290_T94(int8_t cs = -1, int8_t dc = -1, int8_t rst = -1, int8_t busy = -1)
    : GxEPD2_BW_Base(296, 128) { (void)cs; (void)dc; (void)rst; (void)busy; }
};

// Template wrapper matching GxEPD2 API: GxEPD2_BW<GxEPD2_154_D67, ...>
template<typename GxEPD2_Type, int PAGE_HEIGHT = 16>
class GxEPD2_BW : public GxEPD2_Type {
public:
    using GxEPD2_Type::GxEPD2_Type;
};
