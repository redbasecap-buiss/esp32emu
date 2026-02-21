// BNO055 absolute orientation sensor — read Euler angles and calibration
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>

Adafruit_BNO055 bno(55, 0x28);

void setup() {
    Serial.begin(115200);
    Wire.begin();
    if (!bno.begin(OPERATION_MODE_NDOF)) {
        Serial.println("BNO055 not detected!");
        while (1) delay(100);
    }
    Serial.println("BNO055 initialized — NDOF mode");
    delay(1000);
}

void loop() {
    // Read Euler angles
    auto euler = bno.getVector(VECTOR_EULER);
    Serial.print("Heading: "); Serial.print(euler.x());
    Serial.print("  Roll: "); Serial.print(euler.y());
    Serial.print("  Pitch: "); Serial.println(euler.z());

    // Read quaternion
    auto quat = bno.getQuat();
    Serial.print("Quat: w="); Serial.print(quat.w());
    Serial.print(" x="); Serial.print(quat.x());
    Serial.print(" y="); Serial.print(quat.y());
    Serial.print(" z="); Serial.println(quat.z());

    // Check calibration
    uint8_t sys, gyro, accel, mag;
    bno.getCalibration(&sys, &gyro, &accel, &mag);
    Serial.print("Cal: sys="); Serial.print(sys);
    Serial.print(" gyro="); Serial.print(gyro);
    Serial.print(" accel="); Serial.print(accel);
    Serial.print(" mag="); Serial.println(mag);

    if (bno.isFullyCalibrated()) {
        Serial.println("✅ Fully calibrated!");
    }

    Serial.print("Temp: "); Serial.print(bno.getTemp()); Serial.println(" °C");
    Serial.println("---");
    delay(500);
}
