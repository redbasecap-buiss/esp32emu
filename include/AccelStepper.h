// esp32emu – AccelStepper library mock
#pragma once
#include <cstdint>
#include <cmath>
#include <algorithm>

class AccelStepper {
public:
    enum MotorInterfaceType {
        FUNCTION = 0,
        DRIVER = 1,
        FULL2WIRE = 2,
        FULL3WIRE = 3,
        FULL4WIRE = 4,
        HALF3WIRE = 6,
        HALF4WIRE = 8
    };

    AccelStepper(uint8_t interface_ = FULL4WIRE, int pin1 = 2, int pin2 = 3,
                 int pin3 = 4, int pin4 = 5, bool enable = true)
        : _interface(interface_), _pin1(pin1), _pin2(pin2), _pin3(pin3), _pin4(pin4),
          _currentPos(0), _targetPos(0), _speed(0), _maxSpeed(1000),
          _acceleration(100), _running(false) {
        (void)enable;
    }

    void moveTo(long absolute) { _targetPos = absolute; }
    void move(long relative) { _targetPos = _currentPos + relative; }

    bool run() {
        if (_currentPos == _targetPos) return false;
        if (_currentPos < _targetPos) _currentPos++;
        else _currentPos--;
        _running = (_currentPos != _targetPos);
        return _running;
    }

    bool runSpeed() {
        if (_speed > 0) _currentPos++;
        else if (_speed < 0) _currentPos--;
        return _speed != 0;
    }

    void runToPosition() {
        while (run()) {}
    }

    void runToNewPosition(long position) {
        moveTo(position);
        runToPosition();
    }

    void stop() {
        _targetPos = _currentPos;
        _running = false;
    }

    void setMaxSpeed(float speed) { _maxSpeed = std::abs(speed); }
    float maxSpeed() const { return _maxSpeed; }

    void setAcceleration(float acceleration) { _acceleration = std::abs(acceleration); }
    float acceleration() const { return _acceleration; }

    void setSpeed(float speed) {
        _speed = speed;
        if (std::abs(_speed) > _maxSpeed) _speed = (_speed > 0) ? _maxSpeed : -_maxSpeed;
    }
    float speed() const { return _speed; }

    long distanceToGo() const { return _targetPos - _currentPos; }
    long targetPosition() const { return _targetPos; }
    long currentPosition() const { return _currentPos; }

    void setCurrentPosition(long position) {
        _currentPos = position;
        _targetPos = position;
    }

    void enableOutputs() { _enabled = true; }
    void disableOutputs() { _enabled = false; }
    bool isRunning() const { return _currentPos != _targetPos; }

    // Test helpers
    int _getInterface() const { return _interface; }
    bool _isEnabled() const { return _enabled; }

private:
    uint8_t _interface;
    int _pin1, _pin2, _pin3, _pin4;
    long _currentPos;
    long _targetPos;
    float _speed;
    float _maxSpeed;
    float _acceleration;
    bool _running = false;
    bool _enabled = true;
};
