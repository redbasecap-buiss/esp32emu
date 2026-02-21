#include <Arduino.h>
#include <DRV8825.h>

// DRV8825 stepper driver: STEP=2, DIR=3, EN=4
DRV8825 stepper(2, 3, 4);

void setup() {
    Serial.begin(115200);
    Serial.println("DRV8825 Stepper Motor Driver");

    stepper.begin(120, 16); // 120 RPM, 1/16 microstepping
    stepper.setStepsPerRevolution(200);
    stepper.enable();

    Serial.println("Moving 360° clockwise...");
    stepper.rotate(360.0f);
    Serial.print("Position: "); Serial.print(stepper.getPosition());
    Serial.print(" steps ("); Serial.print(stepper.getAngle(), 1);
    Serial.println("°)");

    Serial.println("Moving 180° counter-clockwise...");
    stepper.rotate(-180.0f);
    Serial.print("Position: "); Serial.print(stepper.getPosition());
    Serial.print(" steps ("); Serial.print(stepper.getAngle(), 1);
    Serial.println("°)");

    Serial.print("Total steps taken: ");
    Serial.println(stepper.getTotalSteps());
}

void loop() {
    // Oscillate back and forth
    stepper.move(800);
    Serial.print("→ Pos: "); Serial.println(stepper.getPosition());
    delay(1000);

    stepper.move(-800);
    Serial.print("← Pos: "); Serial.println(stepper.getPosition());
    delay(1000);
}
