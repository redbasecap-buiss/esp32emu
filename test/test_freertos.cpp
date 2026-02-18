#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <cassert>
#include <cstdio>
#include <atomic>
#include <chrono>

static std::atomic<int> counter{0};
static SemaphoreHandle_t mtx = nullptr;

void incrementTask(void* param) {
    int times = *(int*)param;
    for (int i = 0; i < times; i++) {
        xSemaphoreTake(mtx, portMAX_DELAY);
        counter++;
        xSemaphoreGive(mtx);
        vTaskDelay(1);
    }
    vTaskDelete(nullptr);
}

int main() {
    // Test xTaskCreate
    TaskHandle_t handle = nullptr;
    int param = 10;
    BaseType_t ret = xTaskCreate(incrementTask, "test_task", 2048, &param, 1, &handle);
    assert(ret == pdPASS);
    assert(handle != nullptr);

    // Test mutex
    mtx = xSemaphoreCreateMutex();
    assert(mtx != nullptr);

    // Test tick count
    TickType_t t1 = xTaskGetTickCount();
    vTaskDelay(10);
    TickType_t t2 = xTaskGetTickCount();
    assert(t2 > t1);

    // Test stack watermark (always returns mock value)
    assert(uxTaskGetStackHighWaterMark(handle) > 0);

    // Wait for first task to finish
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Run two tasks with shared counter protected by mutex
    counter = 0;
    int n = 50;
    TaskHandle_t h1, h2;
    xTaskCreate(incrementTask, "worker1", 2048, &n, 1, &h1);
    xTaskCreate(incrementTask, "worker2", 2048, &n, 1, &h2);

    // Wait for tasks to finish (50 iterations * 1ms + overhead; generous for slow CI)
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    xSemaphoreTake(mtx, portMAX_DELAY);
    int final_count = counter.load();
    xSemaphoreGive(mtx);

    assert(final_count == 100);

    // Test pinned to core
    TaskHandle_t h3;
    int p2 = 5;
    ret = xTaskCreatePinnedToCore(incrementTask, "pinned", 4096, &p2, 2, &h3, 0);
    assert(ret == pdPASS);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    vSemaphoreDelete(mtx);

    // Cleanup
    esp32emu::freertos::cleanupTasks();

    printf("test_freertos: all assertions passed\n");
    return 0;
}
