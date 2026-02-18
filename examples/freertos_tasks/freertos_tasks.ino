// FreeRTOS Multi-Task Example
// Demonstrates task creation, mutex synchronization, and pinned-to-core tasks.
// Run: make build/examples/freertos_tasks && ./build/examples/freertos_tasks

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

SemaphoreHandle_t serialMutex;
int sharedCounter = 0;

void sensorTask(void* param) {
    (void)param;
    while (true) {
        xSemaphoreTake(serialMutex, portMAX_DELAY);
        sharedCounter++;
        Serial.print("[Sensor] Reading #");
        Serial.print(sharedCounter);
        Serial.print(" — value: ");
        Serial.println(analogRead(A0));
        xSemaphoreGive(serialMutex);
        vTaskDelay(500);
    }
}

void blinkTask(void* param) {
    (void)param;
    bool state = false;
    while (true) {
        state = !state;
        digitalWrite(LED_BUILTIN, state ? HIGH : LOW);
        xSemaphoreTake(serialMutex, portMAX_DELAY);
        Serial.print("[Blink] LED ");
        Serial.println(state ? "ON" : "OFF");
        xSemaphoreGive(serialMutex);
        vTaskDelay(1000);
    }
}

void statusTask(void* param) {
    (void)param;
    while (true) {
        xSemaphoreTake(serialMutex, portMAX_DELAY);
        Serial.print("[Status] Uptime: ");
        Serial.print(millis() / 1000);
        Serial.print("s | Readings: ");
        Serial.print(sharedCounter);
        Serial.print(" | Free stack: ");
        Serial.println(uxTaskGetStackHighWaterMark(nullptr));
        xSemaphoreGive(serialMutex);
        vTaskDelay(2000);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    serialMutex = xSemaphoreCreateMutex();

    Serial.println("=== FreeRTOS Multi-Task Demo ===");
    Serial.println("Creating 3 tasks...");

    xTaskCreatePinnedToCore(sensorTask, "SensorTask", 4096, nullptr, 2, nullptr, 0);
    xTaskCreatePinnedToCore(blinkTask,  "BlinkTask",  2048, nullptr, 1, nullptr, 1);
    xTaskCreate(statusTask, "StatusTask", 2048, nullptr, 1, nullptr);

    Serial.println("All tasks running!");
}

void loop() {
    vTaskDelay(10000);
}
