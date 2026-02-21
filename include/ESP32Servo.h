#pragma once
#include "Arduino.h"
#include <cstdio>
#include <map>

class Servo {
public:
    Servo() : _pin(-1), _us(1500), _min(544), _max(2400), _attached(false) {}

    int attach(int pin, int minUs = 544, int maxUs = 2400) {
        _pin = pin;
        _min = minUs;
        _max = maxUs;
        _attached = true;
        printf("[esp32emu] Servo: attached to pin %d (min=%d, max=%d us)\n", pin, minUs, maxUs);
        return pin;
    }

    void detach() {
        if (_attached) {
            printf("[esp32emu] Servo: detached from pin %d\n", _pin);
        }
        _attached = false;
    }

    void write(int angle) {
        if (!_attached) return;
        if (angle < 0) angle = 0;
        if (angle > 180) angle = 180;
        _us = map(angle, 0, 180, _min, _max);
        printf("[esp32emu] Servo pin %d: angle=%d° (pulse=%d us)\n", _pin, angle, _us);
    }

    void writeMicroseconds(int us) {
        if (!_attached) return;
        _us = us;
        int angle = map(us, _min, _max, 0, 180);
        printf("[esp32emu] Servo pin %d: pulse=%d us (angle≈%d°)\n", _pin, us, angle);
    }

    int read() const {
        return map(_us, _min, _max, 0, 180);
    }

    int readMicroseconds() const { return _us; }
    bool attached() const { return _attached; }
    int attachedPin() const { return _pin; }

    // Test helpers
    static void test_reset() {}

private:
    int _pin;
    int _us;
    int _min;
    int _max;
    bool _attached;

    static int map(int x, int in_min, int in_max, int out_min, int out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};
