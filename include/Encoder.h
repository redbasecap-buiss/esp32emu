// esp32emu – Encoder library mock (rotary encoder)
#pragma once
#include <cstdint>
#include <atomic>

class Encoder {
public:
    Encoder(int pinA, int pinB) : _pinA(pinA), _pinB(pinB), _position(0) {}

    int32_t read() const { return _position.load(); }
    void write(int32_t value) { _position.store(value); }

    // Test helpers
    void _setPosition(int32_t pos) { _position.store(pos); }
    void _rotate(int32_t delta) { _position.fetch_add(delta); }
    int pinA() const { return _pinA; }
    int pinB() const { return _pinB; }

private:
    int _pinA, _pinB;
    std::atomic<int32_t> _position;
};
