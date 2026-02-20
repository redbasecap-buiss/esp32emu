#pragma once
// esp32emu: TFT_eSPI display library mock
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>

// Color definitions (RGB565)
#define TFT_BLACK       0x0000
#define TFT_NAVY        0x000F
#define TFT_DARKGREEN   0x03E0
#define TFT_DARKCYAN    0x03EF
#define TFT_MAROON      0x7800
#define TFT_PURPLE      0x780F
#define TFT_OLIVE       0x7BE0
#define TFT_LIGHTGREY   0xD69A
#define TFT_DARKGREY    0x7BEF
#define TFT_BLUE        0x001F
#define TFT_GREEN       0x07E0
#define TFT_CYAN        0x07FF
#define TFT_RED         0xF800
#define TFT_MAGENTA     0xF81F
#define TFT_YELLOW      0xFFE0
#define TFT_WHITE       0xFFFF
#define TFT_ORANGE      0xFDA0
#define TFT_GREENYELLOW 0xB7E0
#define TFT_PINK        0xFE19
#define TFT_BROWN       0x9A60
#define TFT_GOLD        0xFEA0
#define TFT_SILVER      0xC618
#define TFT_SKYBLUE     0x867D
#define TFT_VIOLET      0x915C
#define TFT_TRANSPARENT 0x0120

// Datum positions
#define TL_DATUM  0
#define TC_DATUM  1
#define TR_DATUM  2
#define ML_DATUM  3
#define MC_DATUM  4
#define MR_DATUM  5
#define BL_DATUM  6
#define BC_DATUM  7
#define BR_DATUM  8
#define CL_DATUM  3
#define CC_DATUM  4
#define CR_DATUM  5

class TFT_eSPI {
public:
    TFT_eSPI() : TFT_eSPI(240, 320) {}
    TFT_eSPI(int16_t w, int16_t h) : _initW(w), _initH(h),
        _width(w), _height(h), _rotation(0), _textDatum(TL_DATUM),
        cursor_x(0), cursor_y(0), textsize(1),
        textcolor(TFT_WHITE), textbgcolor(TFT_BLACK),
        _drawCount(0), _fillCount(0), _textCount(0) {}

    void init() { _inited = true; }
    void begin() { init(); }

    // Dimensions
    int16_t width() const { return _width; }
    int16_t height() const { return _height; }

    void setRotation(uint8_t r) {
        _rotation = r & 3;
        if (_rotation & 1) { _width = _initH; _height = _initW; }
        else { _width = _initW; _height = _initH; }
    }
    uint8_t getRotation() const { return _rotation; }

    // Drawing
    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        (void)x;(void)y;(void)color; _drawCount++;
    }

    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
        (void)x0;(void)y0;(void)x1;(void)y1;(void)color; _drawCount++;
    }

    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
        (void)x;(void)y;(void)w;(void)color; _drawCount++;
    }

    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
        (void)x;(void)y;(void)h;(void)color; _drawCount++;
    }

    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        (void)x;(void)y;(void)w;(void)h;(void)color; _drawCount++;
    }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        (void)x;(void)y;(void)w;(void)h;(void)color; _fillCount++;
    }

    void fillScreen(uint16_t color) {
        fillRect(0, 0, _width, _height, color);
    }

    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
        (void)x0;(void)y0;(void)r;(void)color; _drawCount++;
    }

    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
        (void)x0;(void)y0;(void)r;(void)color; _fillCount++;
    }

    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                      int16_t x2, int16_t y2, uint16_t color) {
        (void)x0;(void)y0;(void)x1;(void)y1;(void)x2;(void)y2;(void)color;
        _drawCount++;
    }

    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                      int16_t x2, int16_t y2, uint16_t color) {
        (void)x0;(void)y0;(void)x1;(void)y1;(void)x2;(void)y2;(void)color;
        _fillCount++;
    }

    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
        (void)x;(void)y;(void)w;(void)h;(void)r;(void)color; _drawCount++;
    }

    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
        (void)x;(void)y;(void)w;(void)h;(void)r;(void)color; _fillCount++;
    }

    // Text
    void setCursor(int16_t x, int16_t y) { cursor_x = x; cursor_y = y; }
    void setCursor(int16_t x, int16_t y, uint8_t font) { cursor_x = x; cursor_y = y; (void)font; }
    int16_t getCursorX() const { return cursor_x; }
    int16_t getCursorY() const { return cursor_y; }

    void setTextColor(uint16_t c) { textcolor = c; textbgcolor = TFT_BLACK; }
    void setTextColor(uint16_t c, uint16_t bg) { textcolor = c; textbgcolor = bg; }
    void setTextSize(uint8_t s) { textsize = s; }
    void setTextDatum(uint8_t d) { _textDatum = d; }
    uint8_t getTextDatum() const { return _textDatum; }
    void setTextWrap(bool w, bool /*wrapY*/ = false) { _wrap = w; }
    void setTextFont(uint8_t /*f*/) {}
    void setFreeFont(const void * /*f*/) {}

    int16_t textWidth(const char *str, uint8_t /*font*/ = 1) {
        return (int16_t)(strlen(str) * 6 * textsize);
    }
    int16_t fontHeight(uint8_t /*font*/ = 1) { return 8 * textsize; }

    int16_t drawString(const char *str, int16_t x, int16_t y, uint8_t /*font*/ = 2) {
        (void)x; (void)y;
        _printedText += str;
        _textCount++;
        return textWidth(str);
    }
    int16_t drawString(const std::string &s, int16_t x, int16_t y, uint8_t f = 2) {
        return drawString(s.c_str(), x, y, f);
    }

    int16_t drawNumber(long num, int16_t x, int16_t y, uint8_t font = 2) {
        char buf[16]; snprintf(buf, sizeof(buf), "%ld", num);
        return drawString(buf, x, y, font);
    }

    int16_t drawFloat(float val, uint8_t dp, int16_t x, int16_t y, uint8_t font = 2) {
        char buf[32]; snprintf(buf, sizeof(buf), "%.*f", dp, val);
        return drawString(buf, x, y, font);
    }

    int16_t drawChar(uint16_t c, int16_t x, int16_t y, uint8_t /*font*/ = 2) {
        char buf[2] = {(char)c, 0};
        return drawString(buf, x, y);
    }

    size_t print(const char *s) { _printedText += s; _textCount++; return strlen(s); }
    size_t print(int v) { return print(std::to_string(v).c_str()); }
    size_t print(float v, int d = 2) { char b[32]; snprintf(b,32,"%.*f",d,v); return print(b); }
    size_t println(const char *s = "") { size_t n = print(s); _printedText += '\n'; return n+1; }
    size_t println(int v) { size_t n = print(v); _printedText += '\n'; return n+1; }

    // Sprite-like push
    void pushImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *data) {
        (void)x;(void)y;(void)w;(void)h;(void)data; _drawCount++;
    }

    // Color conversion
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // Backlight
    void writecommand(uint8_t /*c*/) {}
    void writedata(uint8_t /*d*/) {}

    // Invert
    void invertDisplay(bool i) { _inverted = i; }

    // Test helpers
    uint32_t getDrawCount() const { return _drawCount; }
    uint32_t getFillCount() const { return _fillCount; }
    uint32_t getTextCount() const { return _textCount; }
    const std::string& getPrintedText() const { return _printedText; }
    bool isInited() const { return _inited; }
    bool isInverted() const { return _inverted; }
    void resetCounters() { _drawCount=0; _fillCount=0; _textCount=0; _printedText.clear(); }

private:
    int16_t _initW, _initH;
    int16_t _width, _height;
    uint8_t _rotation;
    uint8_t _textDatum;
    int16_t cursor_x, cursor_y;
    uint8_t textsize;
    uint16_t textcolor, textbgcolor;
    bool _wrap = true;
    bool _inited = false;
    bool _inverted = false;
    uint32_t _drawCount, _fillCount, _textCount;
    std::string _printedText;
};

// TFT_eSprite - framebuffer sprite
class TFT_eSprite : public TFT_eSPI {
public:
    TFT_eSprite(TFT_eSPI *tft) : TFT_eSPI(), _tft(tft), _created(false), _buffer(nullptr) {}
    ~TFT_eSprite() { deleteSprite(); }

    void *createSprite(int16_t w, int16_t h) {
        deleteSprite();
        _sprW = w; _sprH = h;
        _buffer = (uint16_t*)calloc(w * h, sizeof(uint16_t));
        _created = _buffer != nullptr;
        return _buffer;
    }

    void deleteSprite() {
        free(_buffer);
        _buffer = nullptr;
        _created = false;
    }

    void pushSprite(int16_t x, int16_t y) {
        if (_tft && _created)
            _tft->pushImage(x, y, _sprW, _sprH, _buffer);
    }

    void pushSprite(int16_t x, int16_t y, uint16_t /*transparent*/) {
        pushSprite(x, y);
    }

    bool isCreated() const { return _created; }
    int16_t spriteWidth() const { return _sprW; }
    int16_t spriteHeight() const { return _sprH; }

private:
    TFT_eSPI *_tft;
    bool _created;
    uint16_t *_buffer;
    int16_t _sprW = 0, _sprH = 0;
};
