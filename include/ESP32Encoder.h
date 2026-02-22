#pragma once
#include <cstdint>

class ESP32Encoder {
public:
    ESP32Encoder() = default;

    void attachHalfQuad(int aPinNumber, int bPinNumber) {
        _pinA = aPinNumber;
        _pinB = bPinNumber;
        _attached = true;
        _fullQuad = false;
    }

    void attachFullQuad(int aPinNumber, int bPinNumber) {
        _pinA = aPinNumber;
        _pinB = bPinNumber;
        _attached = true;
        _fullQuad = true;
    }

    void attachSingleEdge(int aPinNumber, int bPinNumber) {
        _pinA = aPinNumber;
        _pinB = bPinNumber;
        _attached = true;
    }

    int64_t getCount() const { return _count; }
    int64_t clearCount() { int64_t c = _count; _count = 0; return c; }
    void setCount(int64_t value) { _count = value; }
    void pauseCount() { _paused = true; }
    void resumeCount() { _paused = false; }

    bool isAttached() const { return _attached; }
    bool isFullQuad() const { return _fullQuad; }
    int pinA() const { return _pinA; }
    int pinB() const { return _pinB; }

    // Test helper: simulate encoder ticks
    void _simulateTicks(int64_t ticks) {
        if (!_paused) _count += ticks;
    }

    static void useInternalWeakPullResistors(int type) { _pullType = type; }
    static constexpr int UP = 1;
    static constexpr int DOWN = 2;
    static constexpr int NONE = 0;

private:
    int64_t _count = 0;
    int _pinA = -1;
    int _pinB = -1;
    bool _attached = false;
    bool _fullQuad = false;
    bool _paused = false;
    static inline int _pullType = 0;
};
