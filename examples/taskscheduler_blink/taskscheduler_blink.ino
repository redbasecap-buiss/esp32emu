// TaskScheduler — Cooperative Multitasking
// Multiple tasks running at different intervals without delay().

#include <Arduino.h>
#include <TaskScheduler.h>

Scheduler runner;

#define LED1 2
#define LED2 4

bool led1State = false, led2State = false;
int sensorValue = 0;

void blinkLed1() {
    led1State = !led1State;
    digitalWrite(LED1, led1State);
    Serial.print("💡 LED1: ");
    Serial.println(led1State ? "ON" : "OFF");
}

void blinkLed2() {
    led2State = !led2State;
    digitalWrite(LED2, led2State);
    Serial.print("💡 LED2: ");
    Serial.println(led2State ? "ON" : "OFF");
}

void readSensor() {
    sensorValue = analogRead(A0);
    Serial.print("📊 Sensor: ");
    Serial.println(sensorValue);
}

void statusReport() {
    Serial.println("═══════════════════");
    Serial.print("LED1="); Serial.print(led1State);
    Serial.print(" LED2="); Serial.print(led2State);
    Serial.print(" Sensor="); Serial.println(sensorValue);
    Serial.println("═══════════════════");
}

Task tBlink1(500, TASK_FOREVER, blinkLed1);
Task tBlink2(1000, TASK_FOREVER, blinkLed2);
Task tSensor(2000, TASK_FOREVER, readSensor);
Task tStatus(5000, TASK_FOREVER, statusReport);

void setup() {
    Serial.begin(115200);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);

    runner.init();
    runner.addTask(tBlink1);
    runner.addTask(tBlink2);
    runner.addTask(tSensor);
    runner.addTask(tStatus);

    tBlink1.enable();
    tBlink2.enable();
    tSensor.enable();
    tStatus.enable();

    Serial.println("⏱️  TaskScheduler Demo — 4 concurrent tasks");
}

void loop() {
    runner.execute();
}
