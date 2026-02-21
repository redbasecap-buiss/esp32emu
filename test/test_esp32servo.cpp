#include "ESP32Servo.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test attach/detach
    Servo s;
    assert(!s.attached());
    s.attach(13);
    assert(s.attached());
    assert(s.attachedPin() == 13);

    // Test write angle
    s.write(0);
    assert(s.read() == 0);
    s.write(90);
    assert(s.read() == 90);
    s.write(180);
    assert(s.read() == 180);

    // Test clamping
    s.write(-10);
    assert(s.read() == 0);
    s.write(200);
    assert(s.read() == 180);

    // Test microseconds
    s.writeMicroseconds(1500);
    int angle = s.read();
    assert(angle >= 88 && angle <= 92); // ~90 degrees

    // Test custom range
    Servo s2;
    s2.attach(14, 500, 2500);
    s2.write(90);
    assert(s2.readMicroseconds() == 1500);

    // Test detach
    s.detach();
    assert(!s.attached());

    printf("test_esp32servo: all assertions passed\n");
    return 0;
}
