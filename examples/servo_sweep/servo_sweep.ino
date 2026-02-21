#include <ESP32Servo.h>

Servo myServo;
int pos = 0;

void setup() {
    Serial.begin(115200);
    myServo.attach(13);
    Serial.println("Servo sweep example");
}

void loop() {
    for (pos = 0; pos <= 180; pos++) {
        myServo.write(pos);
        delay(15);
    }
    for (pos = 180; pos >= 0; pos--) {
        myServo.write(pos);
        delay(15);
    }
}
