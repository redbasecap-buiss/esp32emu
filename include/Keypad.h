// esp32emu — Keypad mock: Matrix keypad library
#pragma once
#include <cstdint>
#include <cstring>
#include <functional>

#define NO_KEY '\0'
#define IDLE 0
#define PRESSED 1
#define HOLD 2
#define RELEASED 3

class Keypad {
public:
    Keypad(char* userKeymap, uint8_t* rowPins, uint8_t* colPins, uint8_t numRows, uint8_t numCols)
        : _rows(numRows), _cols(numCols), _lastKey(NO_KEY), _state(IDLE), _holdTime(500),
          _debounceTime(50), _injectedKey(NO_KEY), _listener(nullptr) {
        int mapSize = numRows * numCols;
        _keymap = new char[mapSize];
        memcpy(_keymap, userKeymap, mapSize);
        _rowPins = new uint8_t[numRows];
        _colPins = new uint8_t[numCols];
        memcpy(_rowPins, rowPins, numRows);
        memcpy(_colPins, colPins, numCols);
    }
    ~Keypad() { delete[] _keymap; delete[] _rowPins; delete[] _colPins; }

    char getKey() {
        if (_injectedKey != NO_KEY) {
            char k = _injectedKey;
            _injectedKey = NO_KEY;
            _lastKey = k;
            _state = PRESSED;
            if (_listener) _listener(k);
            return k;
        }
        return NO_KEY;
    }

    char waitForKey() { return getKey(); }

    uint8_t getState() const { return _state; }
    bool isPressed(char k) const { return _lastKey == k && _state == PRESSED; }

    void setHoldTime(unsigned int t) { _holdTime = t; }
    void setDebounceTime(unsigned int t) { _debounceTime = t; }
    unsigned int getHoldTime() const { return _holdTime; }
    unsigned int getDebounceTime() const { return _debounceTime; }

    void addEventListener(void (*listener)(char)) { _listener = listener; }

    bool keyStateChanged() const { return _stateChanged; }

    // --- Test helpers ---
    void test_injectKey(char k) { _injectedKey = k; _stateChanged = true; }
    void test_setState(uint8_t s) { _state = s; _stateChanged = true; }
    char test_getLastKey() const { return _lastKey; }
    uint8_t test_getRows() const { return _rows; }
    uint8_t test_getCols() const { return _cols; }
    char test_getKeymap(int r, int c) const { return _keymap[r * _cols + c]; }

private:
    uint8_t _rows, _cols;
    char* _keymap;
    uint8_t* _rowPins;
    uint8_t* _colPins;
    char _lastKey;
    uint8_t _state;
    unsigned int _holdTime;
    unsigned int _debounceTime;
    char _injectedKey;
    bool _stateChanged = false;
    void (*_listener)(char);
};
