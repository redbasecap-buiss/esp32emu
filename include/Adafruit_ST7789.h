// Adafruit_ST7789.h — ST7789 TFT display mock for esp32emu
#pragma once
#include "Adafruit_GFX.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

class Adafruit_ST7789 : public Adafruit_GFX {
public:
    Adafruit_ST7789(int8_t cs, int8_t dc, int8_t rst = -1)
        : Adafruit_GFX(240, 320), _cs(cs), _dc(dc), _rst(rst),
          _rotation(0), _invert(false), _sleep(false), _brightness(255) {
        _framebuffer.resize(240 * 320, 0);
    }

    Adafruit_ST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst = -1)
        : Adafruit_GFX(240, 320), _cs(cs), _dc(dc), _rst(rst),
          _rotation(0), _invert(false), _sleep(false), _brightness(255) {
        (void)mosi; (void)sclk;
        _framebuffer.resize(240 * 320, 0);
    }

    void init(uint16_t width = 240, uint16_t height = 320) {
        _width = width;
        _height = height;
        _framebuffer.resize(width * height, 0);
        printf("[esp32emu] ST7789: initialized %dx%d (CS=%d, DC=%d)\n", width, height, _cs, _dc);
    }

    void setRotation(uint8_t r) {
        _rotation = r & 3;
        printf("[esp32emu] ST7789: rotation=%d\n", _rotation);
    }

    uint8_t getRotation() const { return _rotation; }

    void invertDisplay(bool i) {
        _invert = i;
        printf("[esp32emu] ST7789: invert=%s\n", i ? "on" : "off");
    }

    bool getInvert() const { return _invert; }

    void enableSleep(bool s) {
        _sleep = s;
        printf("[esp32emu] ST7789: sleep=%s\n", s ? "on" : "off");
    }

    bool isSleeping() const { return _sleep; }

    void enableDisplay(bool e) {
        printf("[esp32emu] ST7789: display=%s\n", e ? "on" : "off");
    }

    void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
        _addrX = x; _addrY = y; _addrW = w; _addrH = h;
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        if (x >= 0 && x < _width && y >= 0 && y < _height) {
            _framebuffer[y * _width + x] = color;
        }
    }

    void fillScreen(uint16_t color) override {
        std::fill(_framebuffer.begin(), _framebuffer.end(), color);
    }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        for (int16_t j = y; j < y + h && j < _height; j++) {
            for (int16_t i = x; i < x + w && i < _width; i++) {
                if (i >= 0 && j >= 0) _framebuffer[j * _width + i] = color;
            }
        }
    }

    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
        fillRect(x, y, w, 1, color);
    }

    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
        fillRect(x, y, 1, h, color);
    }

    void setSPISpeed(uint32_t freq) { _spiFreq = freq; }
    uint32_t getSPISpeed() const { return _spiFreq; }

    // Color conversion helpers
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // Test helpers
    uint16_t getPixel(int16_t x, int16_t y) const {
        if (x >= 0 && x < _width && y >= 0 && y < _height)
            return _framebuffer[y * _width + x];
        return 0;
    }

    const std::vector<uint16_t>& getFramebuffer() const { return _framebuffer; }

private:
    int8_t _cs, _dc, _rst;
    uint8_t _rotation;
    bool _invert, _sleep;
    uint8_t _brightness;
    uint32_t _spiFreq = 40000000;
    uint16_t _addrX = 0, _addrY = 0, _addrW = 0, _addrH = 0;
    std::vector<uint16_t> _framebuffer;
};
