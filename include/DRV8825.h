#pragma once
#include "Arduino.h"

// DRV8825 / A4988 stepper motor driver mock
// Controls stepper via STEP + DIR pins with microstepping support

class DRV8825 {
public:
    DRV8825(uint8_t stepPin, uint8_t dirPin, uint8_t enablePin = 255,
            uint8_t ms1Pin = 255, uint8_t ms2Pin = 255, uint8_t ms3Pin = 255)
        : _stepPin(stepPin), _dirPin(dirPin), _enablePin(enablePin),
          _ms1(ms1Pin), _ms2(ms2Pin), _ms3(ms3Pin) {}

    void begin(float rpm = 60, uint8_t microsteps = 1) {
        _rpm = rpm;
        _microsteps = microsteps;
        _initialized = true;
        if (_enablePin != 255) pinMode(_enablePin, OUTPUT);
        pinMode(_stepPin, OUTPUT);
        pinMode(_dirPin, OUTPUT);
    }

    void setRPM(float rpm) { _rpm = rpm; }
    float getRPM() const { return _rpm; }

    void setMicrostep(uint8_t microsteps) { _microsteps = microsteps; }
    uint8_t getMicrostep() const { return _microsteps; }

    void setStepsPerRevolution(int steps) { _stepsPerRev = steps; }
    int getStepsPerRevolution() const { return _stepsPerRev; }

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }
    bool isEnabled() const { return _enabled; }

    // Move by number of full steps (positive = CW, negative = CCW)
    void move(long steps) {
        if (steps > 0) _direction = 1;
        else if (steps < 0) _direction = -1;
        _position += steps;
        _totalSteps += abs(steps);
    }

    // Rotate by degrees
    void rotate(float degrees) {
        long steps = (long)(degrees / 360.0f * _stepsPerRev * _microsteps);
        move(steps);
    }

    // Rotate full revolutions
    void rotate(int revolutions) {
        move((long)revolutions * _stepsPerRev * _microsteps);
    }

    long getPosition() const { return _position; }
    float getAngle() const {
        return (float)_position / (_stepsPerRev * _microsteps) * 360.0f;
    }

    int getDirection() const { return _direction; }
    unsigned long getTotalSteps() const { return _totalSteps; }

    void resetPosition() { _position = 0; }

    // ── Test helpers ──
    bool _isInitialized() const { return _initialized; }
    uint8_t _getStepPin() const { return _stepPin; }
    uint8_t _getDirPin() const { return _dirPin; }

private:
    uint8_t _stepPin, _dirPin, _enablePin;
    uint8_t _ms1, _ms2, _ms3;
    bool _initialized = false;
    bool _enabled = true;
    float _rpm = 60;
    uint8_t _microsteps = 1;
    int _stepsPerRev = 200; // Standard 1.8° stepper
    long _position = 0;
    int _direction = 0; // -1=CCW, 0=idle, 1=CW
    unsigned long _totalSteps = 0;
};
