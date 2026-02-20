#pragma once
// esp32emu: Adafruit_GFX base graphics library mock
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <string>

// Basic color definitions
#define GFX_BLACK   0x0000
#define GFX_WHITE   0xFFFF
#define GFX_RED     0xF800
#define GFX_GREEN   0x07E0
#define GFX_BLUE    0x001F
#define GFX_YELLOW  0xFFE0
#define GFX_CYAN    0x07FF
#define GFX_MAGENTA 0xF81F

struct GFXfont;

class Adafruit_GFX {
public:
    Adafruit_GFX(int16_t w, int16_t h) : WIDTH(w), HEIGHT(h),
        _width(w), _height(h), _rotation(0),
        cursor_x(0), cursor_y(0), textsize_x(1), textsize_y(1),
        textcolor(GFX_WHITE), textbgcolor(GFX_BLACK), wrap(true),
        _cp437(false), gfxFont(nullptr),
        _pixelCount(0), _fillRectCount(0), _lineCount(0), _circleCount(0) {}

    virtual ~Adafruit_GFX() {}

    // Subclasses must implement
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) {
        (void)x; (void)y; (void)color;
        _pixelCount++;
        _lastPixelX = x; _lastPixelY = y; _lastPixelColor = color;
    }

    // Drawing primitives
    virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
        (void)x0; (void)y0; (void)x1; (void)y1; (void)color;
        _lineCount++;
    }

    virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        drawLine(x, y, x+w-1, y, color);
        drawLine(x, y+h-1, x+w-1, y+h-1, color);
        drawLine(x, y, x, y+h-1, color);
        drawLine(x+w-1, y, x+w-1, y+h-1, color);
    }

    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        (void)x; (void)y; (void)w; (void)h; (void)color;
        _fillRectCount++;
    }

    virtual void fillScreen(uint16_t color) {
        fillRect(0, 0, _width, _height, color);
    }

    virtual void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
        (void)x0; (void)y0; (void)r; (void)color;
        _circleCount++;
    }

    virtual void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
        (void)x0; (void)y0; (void)r; (void)color;
        _circleCount++;
    }

    virtual void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                              int16_t x2, int16_t y2, uint16_t color) {
        drawLine(x0,y0,x1,y1,color);
        drawLine(x1,y1,x2,y2,color);
        drawLine(x2,y2,x0,y0,color);
    }

    virtual void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                               int16_t r, uint16_t color) {
        (void)x;(void)y;(void)w;(void)h;(void)r;(void)color;
    }

    virtual void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                               int16_t r, uint16_t color) {
        (void)x;(void)y;(void)w;(void)h;(void)r;(void)color;
    }

    // Text
    void setCursor(int16_t x, int16_t y) { cursor_x = x; cursor_y = y; }
    int16_t getCursorX() const { return cursor_x; }
    int16_t getCursorY() const { return cursor_y; }

    void setTextColor(uint16_t c) { textcolor = c; textbgcolor = c; }
    void setTextColor(uint16_t c, uint16_t bg) { textcolor = c; textbgcolor = bg; }
    void setTextSize(uint8_t s) { textsize_x = s; textsize_y = s; }
    void setTextSize(uint8_t sx, uint8_t sy) { textsize_x = sx; textsize_y = sy; }
    void setTextWrap(bool w) { wrap = w; }
    void cp437(bool x = true) { _cp437 = x; }
    void setFont(const GFXfont *f = nullptr) { gfxFont = f; }

    size_t write(uint8_t c) {
        _printedText += (char)c;
        cursor_x += textsize_x * 6;
        return 1;
    }

    size_t write(const char *str) {
        size_t n = 0;
        while (*str) { write((uint8_t)*str++); n++; }
        return n;
    }

    size_t print(const char *s) { return write(s); }
    size_t print(int val) { return print(std::to_string(val).c_str()); }
    size_t print(float val, int dec = 2) {
        char buf[32]; snprintf(buf, sizeof(buf), "%.*f", dec, val);
        return print(buf);
    }
    size_t println(const char *s = "") { size_t n = print(s); write('\n'); return n+1; }
    size_t println(int val) { size_t n = print(val); write('\n'); return n+1; }
    size_t println(float val, int dec = 2) { size_t n = print(val, dec); write('\n'); return n+1; }

    // Rotation
    void setRotation(uint8_t r) {
        _rotation = r & 3;
        if (_rotation & 1) { _width = HEIGHT; _height = WIDTH; }
        else { _width = WIDTH; _height = HEIGHT; }
    }
    uint8_t getRotation() const { return _rotation; }

    int16_t width() const { return _width; }
    int16_t height() const { return _height; }

    // Color conversion
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // Text bounds
    void getTextBounds(const char *str, int16_t x, int16_t y,
                       int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
        size_t len = strlen(str);
        *x1 = x;
        *y1 = y;
        *w = (uint16_t)(len * 6 * textsize_x);
        *h = (uint16_t)(8 * textsize_y);
    }

    // Test helpers
    uint32_t getPixelCount() const { return _pixelCount; }
    uint32_t getFillRectCount() const { return _fillRectCount; }
    uint32_t getLineCount() const { return _lineCount; }
    uint32_t getCircleCount() const { return _circleCount; }
    int16_t getLastPixelX() const { return _lastPixelX; }
    int16_t getLastPixelY() const { return _lastPixelY; }
    uint16_t getLastPixelColor() const { return _lastPixelColor; }
    const std::string& getPrintedText() const { return _printedText; }
    void resetCounters() { _pixelCount=0; _fillRectCount=0; _lineCount=0; _circleCount=0; _printedText.clear(); }

protected:
    const int16_t WIDTH, HEIGHT;
    int16_t _width, _height;
    uint8_t _rotation;
    int16_t cursor_x, cursor_y;
    uint8_t textsize_x, textsize_y;
    uint16_t textcolor, textbgcolor;
    bool wrap;
    bool _cp437;
    const GFXfont *gfxFont;
    std::string _printedText;
    uint32_t _pixelCount = 0, _fillRectCount = 0, _lineCount = 0, _circleCount = 0;
    int16_t _lastPixelX = 0, _lastPixelY = 0;
    uint16_t _lastPixelColor = 0;
};

// Minimal GFXcanvas16 for framebuffer use
class GFXcanvas16 : public Adafruit_GFX {
public:
    GFXcanvas16(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
        _buffer = (uint16_t*)calloc(w * h, sizeof(uint16_t));
    }
    ~GFXcanvas16() { free(_buffer); }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        if (x >= 0 && x < _width && y >= 0 && y < _height && _buffer) {
            _buffer[y * WIDTH + x] = color;
        }
        Adafruit_GFX::drawPixel(x, y, color);
    }

    uint16_t *getBuffer() { return _buffer; }
    uint16_t getPixel(int16_t x, int16_t y) {
        if (x >= 0 && x < _width && y >= 0 && y < _height && _buffer)
            return _buffer[y * WIDTH + x];
        return 0;
    }

private:
    uint16_t *_buffer;
};
