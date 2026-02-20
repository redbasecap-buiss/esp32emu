// AccelStepper example – move to positions
#include <Arduino.h>
#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, 2, 3);

void setup() {
    Serial.begin(115200);
    stepper.setMaxSpeed(1000);
    stepper.setAcceleration(500);
    stepper.moveTo(200);
    Serial.println("AccelStepper moving to position 200");
}

void loop() {
    if (stepper.run()) {
        if (stepper.currentPosition() % 50 == 0) {
            Serial.print("Position: ");
            Serial.println(stepper.currentPosition());
        }
    } else {
        // Reverse direction
        if (stepper.currentPosition() == 200)
            stepper.moveTo(0);
        else
            stepper.moveTo(200);
    }
    delay(1);
}
