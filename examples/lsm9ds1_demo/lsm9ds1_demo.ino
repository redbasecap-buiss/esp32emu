// LSM9DS1 9-DOF IMU demo
#include <Adafruit_LSM9DS1.h>

Adafruit_LSM9DS1 lsm;

void setup() {
    Serial.begin(115200);
    if (!lsm.begin()) {
        Serial.println("LSM9DS1 not found!");
        while (1) delay(10);
    }
    lsm.setupAccel(LSM9DS1_ACCELRANGE_4G);
    lsm.setupGyro(LSM9DS1_GYROSCALE_500DPS);
    lsm.setupMag(LSM9DS1_MAGGAIN_8GAUSS);
    Serial.println("LSM9DS1 ready!");
}

void loop() {
    sensors_event_t accel, mag, gyro, temp;
    lsm.getEvent(&accel, &mag, &gyro, &temp);

    Serial.print("Accel: ");
    Serial.print(accel.acceleration.x, 2); Serial.print(", ");
    Serial.print(accel.acceleration.y, 2); Serial.print(", ");
    Serial.println(accel.acceleration.z, 2);

    Serial.print("Gyro:  ");
    Serial.print(gyro.gyro.x, 2); Serial.print(", ");
    Serial.print(gyro.gyro.y, 2); Serial.print(", ");
    Serial.println(gyro.gyro.z, 2);

    Serial.print("Mag:   ");
    Serial.print(mag.magnetic.x, 2); Serial.print(", ");
    Serial.print(mag.magnetic.y, 2); Serial.print(", ");
    Serial.println(mag.magnetic.z, 2);

    Serial.print("Temp:  ");
    Serial.println(temp.temperature, 1);
    Serial.println("---");
    delay(1000);
}
