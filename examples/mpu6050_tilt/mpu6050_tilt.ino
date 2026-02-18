// MPU6050 Tilt Sensor
// Reads accelerometer/gyroscope and displays tilt angles

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    mpu.begin();
    Serial.println("MPU6050 Tilt Sensor Ready");
}

void loop() {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    float pitch = mpu.getPitch();
    float roll = mpu.getRoll();
    float temp = mpu.getTemperature();

    Serial.print("Pitch: ");
    Serial.print(pitch, 1);
    Serial.print("°  Roll: ");
    Serial.print(roll, 1);
    Serial.print("°  Temp: ");
    Serial.print(temp, 1);
    Serial.println("°C");

    Serial.print("  Accel g: X=");
    Serial.print(mpu.getAccelG_X(), 3);
    Serial.print(" Y=");
    Serial.print(mpu.getAccelG_Y(), 3);
    Serial.print(" Z=");
    Serial.println(mpu.getAccelG_Z(), 3);

    delay(500);
}
