#pragma once
// esp32emu — FreeRTOS mock using std::thread
// Provides lightweight emulation of xTaskCreate, vTaskDelay, mutexes, and semaphores.

#include <cstdint>
#include <cstdio>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>
#include <vector>
#include <atomic>

typedef void* TaskHandle_t;
typedef void (*TaskFunction_t)(void*);
typedef uint32_t TickType_t;
typedef int BaseType_t;
typedef unsigned int UBaseType_t;

#define pdPASS    1
#define pdFAIL    0
#define pdTRUE    1
#define pdFALSE   0
#define portMAX_DELAY 0xFFFFFFFF
#define configMAX_PRIORITIES 25
#define portTICK_PERIOD_MS 1
#define tskIDLE_PRIORITY 0

// Semaphore / Mutex types
typedef void* SemaphoreHandle_t;
typedef void* QueueHandle_t;

namespace esp32emu {
namespace freertos {

struct TaskRecord {
    std::thread thread;
    std::string name;
    std::atomic<bool> running{true};
};

inline std::vector<TaskRecord*>& taskList() {
    static std::vector<TaskRecord*> tasks;
    return tasks;
}

inline std::mutex& taskListMutex() {
    static std::mutex mtx;
    return mtx;
}

inline void cleanupTasks() {
    std::lock_guard<std::mutex> lock(taskListMutex());
    for (auto* t : taskList()) {
        t->running = false;
        if (t->thread.joinable()) t->thread.detach();
        delete t;
    }
    taskList().clear();
}

} // namespace freertos
} // namespace esp32emu

inline BaseType_t xTaskCreate(
    TaskFunction_t pvTaskCode,
    const char* pcName,
    uint32_t usStackDepth,
    void* pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t* pxCreatedTask)
{
    (void)usStackDepth;
    (void)uxPriority;

    auto* rec = new esp32emu::freertos::TaskRecord();
    rec->name = pcName ? pcName : "task";
    rec->thread = std::thread([pvTaskCode, pvParameters, rec]() {
        fprintf(stderr, "[esp32emu] Task '%s' started (tid=%p)\n", rec->name.c_str(), (void*)rec);
        pvTaskCode(pvParameters);
        fprintf(stderr, "[esp32emu] Task '%s' ended\n", rec->name.c_str());
    });

    if (pxCreatedTask) *pxCreatedTask = (TaskHandle_t)rec;

    {
        std::lock_guard<std::mutex> lock(esp32emu::freertos::taskListMutex());
        esp32emu::freertos::taskList().push_back(rec);
    }

    fprintf(stderr, "[esp32emu] xTaskCreate: '%s' (priority %u, stack %u)\n", rec->name.c_str(), uxPriority, usStackDepth);
    return pdPASS;
}

inline BaseType_t xTaskCreatePinnedToCore(
    TaskFunction_t pvTaskCode,
    const char* pcName,
    uint32_t usStackDepth,
    void* pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t* pxCreatedTask,
    int coreId)
{
    (void)coreId;
    fprintf(stderr, "[esp32emu] xTaskCreatePinnedToCore: core=%d (ignored, using host thread)\n", coreId);
    return xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
}

inline void vTaskDelay(TickType_t xTicksToDelay) {
    std::this_thread::sleep_for(std::chrono::milliseconds(xTicksToDelay * portTICK_PERIOD_MS));
}

inline void vTaskDelete(TaskHandle_t xTask) {
    if (xTask == nullptr) {
        // Delete calling task — just return (thread will end)
        fprintf(stderr, "[esp32emu] vTaskDelete(NULL) — current task ending\n");
        return;
    }
    auto* rec = static_cast<esp32emu::freertos::TaskRecord*>(xTask);
    rec->running = false;
    fprintf(stderr, "[esp32emu] vTaskDelete: '%s'\n", rec->name.c_str());
}

inline TickType_t xTaskGetTickCount() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return (TickType_t)std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

inline UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask) {
    (void)xTask;
    return 4096; // mock: always plenty of stack
}

// --- Mutex ---
inline SemaphoreHandle_t xSemaphoreCreateMutex() {
    auto* mtx = new std::mutex();
    fprintf(stderr, "[esp32emu] xSemaphoreCreateMutex: %p\n", (void*)mtx);
    return (SemaphoreHandle_t)mtx;
}

inline BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait) {
    (void)xTicksToWait;
    auto* mtx = static_cast<std::mutex*>(xSemaphore);
    mtx->lock();
    return pdTRUE;
}

inline BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore) {
    auto* mtx = static_cast<std::mutex*>(xSemaphore);
    mtx->unlock();
    return pdTRUE;
}

inline void vSemaphoreDelete(SemaphoreHandle_t xSemaphore) {
    auto* mtx = static_cast<std::mutex*>(xSemaphore);
    delete mtx;
}
