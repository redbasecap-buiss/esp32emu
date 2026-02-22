// esp32emu — USBHIDKeyboard mock: ESP32-S2/S3 USB HID keyboard
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

#define KEY_LEFT_CTRL   0x80
#define KEY_LEFT_SHIFT  0x81
#define KEY_LEFT_ALT    0x82
#define KEY_LEFT_GUI    0x83
#define KEY_RIGHT_CTRL  0x84
#define KEY_RIGHT_SHIFT 0x85
#define KEY_RIGHT_ALT   0x86
#define KEY_RIGHT_GUI   0x87
#define KEY_UP_ARROW    0xDA
#define KEY_DOWN_ARROW  0xD9
#define KEY_LEFT_ARROW  0xD8
#define KEY_RIGHT_ARROW 0xD7
#define KEY_BACKSPACE   0xB2
#define KEY_TAB         0xB3
#define KEY_RETURN      0xB0
#define KEY_ESC         0xB1
#define KEY_INSERT      0xD1
#define KEY_DELETE      0xD4
#define KEY_PAGE_UP     0xD3
#define KEY_PAGE_DOWN   0xD6
#define KEY_HOME        0xD2
#define KEY_END         0xD5
#define KEY_CAPS_LOCK   0xC1
#define KEY_F1          0xC2
#define KEY_F2          0xC3
#define KEY_F12         0xCD

class USBHIDKeyboard {
public:
    USBHIDKeyboard() : _begun(false) {}

    void begin() {
        _begun = true;
        memset(_keys, 0, sizeof(_keys));
        _modifiers = 0;
        printf("[esp32emu] USBHIDKeyboard: started\n");
    }

    void end() { _begun = false; }

    size_t press(uint8_t k) {
        if (k >= KEY_LEFT_CTRL && k <= KEY_RIGHT_GUI) {
            _modifiers |= (1 << (k - KEY_LEFT_CTRL));
        } else {
            for (int i = 0; i < 6; i++) {
                if (_keys[i] == 0) { _keys[i] = k; break; }
            }
        }
        _pressLog.push_back(k);
        return 1;
    }

    size_t release(uint8_t k) {
        if (k >= KEY_LEFT_CTRL && k <= KEY_RIGHT_GUI) {
            _modifiers &= ~(1 << (k - KEY_LEFT_CTRL));
        } else {
            for (int i = 0; i < 6; i++) {
                if (_keys[i] == k) { _keys[i] = 0; break; }
            }
        }
        return 1;
    }

    void releaseAll() {
        memset(_keys, 0, sizeof(_keys));
        _modifiers = 0;
    }

    size_t write(uint8_t k) {
        press(k);
        release(k);
        return 1;
    }

    size_t print(const char* s) {
        size_t n = 0;
        while (*s) { write(*s++); n++; }
        _printLog += std::string(s - n, n);
        return n;
    }

    size_t println(const char* s = "") {
        size_t n = print(s);
        write('\n');
        return n + 1;
    }

    // --- Test helpers ---
    bool test_isBegun() const { return _begun; }
    uint8_t test_getModifiers() const { return _modifiers; }
    bool test_isPressed(uint8_t k) const {
        for (int i = 0; i < 6; i++) if (_keys[i] == k) return true;
        return false;
    }
    const std::vector<uint8_t>& test_getPressLog() const { return _pressLog; }
    void test_clearLog() { _pressLog.clear(); _printLog.clear(); }

private:
    bool _begun;
    uint8_t _keys[6];
    uint8_t _modifiers;
    std::vector<uint8_t> _pressLog;
    std::string _printLog;
};
