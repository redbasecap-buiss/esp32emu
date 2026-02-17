#pragma once
#include <cstdint>
#include <cstdio>
#include "esp32emu_terminal.h"

class Servo {
public:
    uint8_t attach(int pin) {
        pin_ = pin;
        attached_ = true;
        fprintf(stderr, "[esp32emu] Servo attached to pin %d\n", pin);
        return pin;
    }

    uint8_t attach(int pin, int min, int max) {
        min_us_ = min;
        max_us_ = max;
        return attach(pin);
    }

    void detach() {
        fprintf(stderr, "[esp32emu] Servo detached from pin %d\n", pin_);
        attached_ = false;
    }

    void write(int angle) {
        if (!attached_) return;
        if (angle < 0) angle = 0;
        if (angle > 180) angle = 180;
        angle_ = angle;
        esp32emu::printServo(angle_);
    }

    void writeMicroseconds(int us) {
        if (!attached_) return;
        int angle = (us - min_us_) * 180 / (max_us_ - min_us_);
        write(angle);
    }

    int read() const { return angle_; }
    int readMicroseconds() const {
        return min_us_ + (long)angle_ * (max_us_ - min_us_) / 180;
    }
    bool attached() const { return attached_; }

private:
    int pin_ = -1;
    int angle_ = 0;
    int min_us_ = 544;
    int max_us_ = 2400;
    bool attached_ = false;
};
