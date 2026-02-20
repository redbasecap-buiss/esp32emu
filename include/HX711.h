#pragma once
// esp32emu: HX711 load cell amplifier mock
#include <cstdint>
#include <cstdio>

class HX711 {
public:
    HX711() : _dout(0), _sck(0), _begun(false), _gain(128),
        _raw(0), _offset(0), _scale(1.0f), _powered(true) {}

    void begin(uint8_t dout, uint8_t sck, uint8_t gain = 128) {
        _dout = dout;
        _sck = sck;
        _gain = gain;
        _begun = true;
    }

    bool is_ready() { return _begun && _powered; }
    bool wait_ready(unsigned long /*timeout_ms*/ = 1000) { return is_ready(); }
    bool wait_ready_retry(int /*retries*/ = 3, unsigned long /*delay_ms*/ = 0) { return is_ready(); }
    bool wait_ready_timeout(unsigned long /*timeout*/ = 1000, unsigned long /*delay_ms*/ = 0) { return is_ready(); }

    long read() { return _raw; }

    long read_average(uint8_t times = 10) {
        (void)times;
        return _raw;
    }

    double get_value(uint8_t times = 1) {
        (void)times;
        return (double)(_raw - _offset);
    }

    float get_units(uint8_t times = 1) {
        (void)times;
        return (float)(_raw - _offset) / _scale;
    }

    void tare(uint8_t times = 10) {
        (void)times;
        _offset = _raw;
    }

    void set_scale(float scale = 1.0f) { _scale = scale; }
    float get_scale() { return _scale; }

    void set_offset(long offset) { _offset = offset; }
    long get_offset() { return _offset; }

    void set_gain(uint8_t gain) { _gain = gain; }
    uint8_t get_gain() { return _gain; }

    void power_down() { _powered = false; }
    void power_up() { _powered = true; }

    // Test helpers
    void setRawValue(long raw) { _raw = raw; }
    long getRawValue() const { return _raw; }
    bool isBegun() const { return _begun; }
    bool isPowered() const { return _powered; }
    uint8_t getDoutPin() const { return _dout; }
    uint8_t getSckPin() const { return _sck; }

private:
    uint8_t _dout, _sck;
    bool _begun;
    uint8_t _gain;
    long _raw;
    long _offset;
    float _scale;
    bool _powered;
};
