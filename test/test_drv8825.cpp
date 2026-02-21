#include "DRV8825.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    printf("test_drv8825: ");

    DRV8825 stepper(2, 3, 4);
    stepper.begin(120, 1);
    assert(stepper._isInitialized());
    assert(stepper._getStepPin() == 2);
    assert(stepper._getDirPin() == 3);

    // RPM
    stepper.setRPM(200);
    assert(fabs(stepper.getRPM() - 200.0f) < 0.01f);

    // Microstepping
    stepper.setMicrostep(16);
    assert(stepper.getMicrostep() == 16);

    // Move forward
    stepper.move(100);
    assert(stepper.getPosition() == 100);
    assert(stepper.getDirection() == 1);
    assert(stepper.getTotalSteps() == 100);

    // Move backward
    stepper.move(-50);
    assert(stepper.getPosition() == 50);
    assert(stepper.getDirection() == -1);
    assert(stepper.getTotalSteps() == 150);

    // Angle calculation (200 steps/rev, 16 microsteps)
    stepper.resetPosition();
    stepper.setMicrostep(1);
    stepper.setStepsPerRevolution(200);
    stepper.move(100); // half revolution
    float angle = stepper.getAngle();
    assert(fabs(angle - 180.0f) < 0.1f);

    // Rotate by degrees
    stepper.resetPosition();
    stepper.rotate(90.0f); // 50 steps for 90° at 200 steps/rev, 1 microstep
    assert(stepper.getPosition() == 50);

    // Enable/disable
    stepper.disable();
    assert(!stepper.isEnabled());
    stepper.enable();
    assert(stepper.isEnabled());

    printf("all assertions passed\n");
    return 0;
}
