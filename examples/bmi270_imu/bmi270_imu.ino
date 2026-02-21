#include <Wire.h>
#include <SparkFunBMI270.h>

BMI270 imu;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (imu.beginI2C() != 0) {
        Serial.println("BMI270 init failed!");
        while (1);
    }

    imu.enableStepCounter();
    Serial.println("BMI270 IMU ready");
}

void loop() {
    imu.getSensorData();

    Serial.print("Accel: X=");
    Serial.print(imu.accX());
    Serial.print(" Y=");
    Serial.print(imu.accY());
    Serial.print(" Z=");
    Serial.println(imu.accZ());

    Serial.print("Gyro: X=");
    Serial.print(imu.gyrX());
    Serial.print(" Y=");
    Serial.print(imu.gyrY());
    Serial.print(" Z=");
    Serial.println(imu.gyrZ());

    Serial.print("Steps: ");
    Serial.println(imu.getStepCount());

    delay(100);
}
