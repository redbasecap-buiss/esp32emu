// Test: AccelStepper library mock
#include "AccelStepper.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    AccelStepper stepper(AccelStepper::DRIVER, 2, 3);
    assert(stepper.currentPosition() == 0);

    // Move to position
    stepper.moveTo(10);
    assert(stepper.targetPosition() == 10);
    assert(stepper.distanceToGo() == 10);
    assert(stepper.isRunning());

    // Run steps
    stepper.run();
    assert(stepper.currentPosition() == 1);
    assert(stepper.distanceToGo() == 9);

    // Run to completion
    stepper.runToPosition();
    assert(stepper.currentPosition() == 10);
    assert(stepper.distanceToGo() == 0);
    assert(!stepper.isRunning());

    // Move relative
    stepper.move(-5);
    assert(stepper.targetPosition() == 5);
    stepper.runToPosition();
    assert(stepper.currentPosition() == 5);

    // Run to new position
    stepper.runToNewPosition(20);
    assert(stepper.currentPosition() == 20);

    // Speed/acceleration
    stepper.setMaxSpeed(500.0);
    assert(std::abs(stepper.maxSpeed() - 500.0f) < 0.01f);
    stepper.setAcceleration(200.0);
    assert(std::abs(stepper.acceleration() - 200.0f) < 0.01f);

    stepper.setSpeed(100.0);
    assert(std::abs(stepper.speed() - 100.0f) < 0.01f);

    // Speed clamped to max
    stepper.setSpeed(999.0);
    assert(std::abs(stepper.speed() - 500.0f) < 0.01f);

    // Stop
    stepper.moveTo(100);
    stepper.stop();
    assert(stepper.distanceToGo() == 0);

    // Set current position
    stepper.setCurrentPosition(42);
    assert(stepper.currentPosition() == 42);
    assert(stepper.targetPosition() == 42);

    // Enable/disable
    stepper.disableOutputs();
    assert(!stepper._isEnabled());
    stepper.enableOutputs();
    assert(stepper._isEnabled());

    printf("test_accelstepper: all assertions passed\n");
    return 0;
}
