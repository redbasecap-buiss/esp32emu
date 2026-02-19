#pragma once
// esp32emu — FreeRTOS Event Groups mock
// Provides xEventGroupCreate, SetBits, WaitBits, ClearBits, GetBits, Delete.

#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cstdio>
#include "FreeRTOS.h"

typedef void* EventGroupHandle_t;
typedef uint32_t EventBits_t;

namespace esp32emu {
namespace event_groups {

struct EventGroup {
    std::mutex mtx;
    std::condition_variable cv;
    EventBits_t bits = 0;
};

} // namespace event_groups
} // namespace esp32emu

inline EventGroupHandle_t xEventGroupCreate() {
    auto* eg = new esp32emu::event_groups::EventGroup();
    fprintf(stderr, "[esp32emu] xEventGroupCreate: %p\n", (void*)eg);
    return (EventGroupHandle_t)eg;
}

inline EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet) {
    auto* eg = static_cast<esp32emu::event_groups::EventGroup*>(xEventGroup);
    std::lock_guard<std::mutex> lock(eg->mtx);
    eg->bits |= uxBitsToSet;
    eg->cv.notify_all();
    return eg->bits;
}

inline EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear) {
    auto* eg = static_cast<esp32emu::event_groups::EventGroup*>(xEventGroup);
    std::lock_guard<std::mutex> lock(eg->mtx);
    EventBits_t prev = eg->bits;
    eg->bits &= ~uxBitsToClear;
    return prev;
}

inline EventBits_t xEventGroupGetBits(EventGroupHandle_t xEventGroup) {
    auto* eg = static_cast<esp32emu::event_groups::EventGroup*>(xEventGroup);
    std::lock_guard<std::mutex> lock(eg->mtx);
    return eg->bits;
}

inline EventBits_t xEventGroupWaitBits(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToWaitFor,
    const BaseType_t xClearOnExit,
    const BaseType_t xWaitForAllBits,
    TickType_t xTicksToWait)
{
    auto* eg = static_cast<esp32emu::event_groups::EventGroup*>(xEventGroup);
    std::unique_lock<std::mutex> lock(eg->mtx);

    auto predicate = [&]() -> bool {
        if (xWaitForAllBits) {
            return (eg->bits & uxBitsToWaitFor) == uxBitsToWaitFor;
        } else {
            return (eg->bits & uxBitsToWaitFor) != 0;
        }
    };

    if (xTicksToWait == portMAX_DELAY) {
        eg->cv.wait(lock, predicate);
    } else if (xTicksToWait > 0) {
        eg->cv.wait_for(lock, std::chrono::milliseconds(xTicksToWait * portTICK_PERIOD_MS), predicate);
    }

    EventBits_t result = eg->bits;
    if (predicate() && xClearOnExit) {
        eg->bits &= ~uxBitsToWaitFor;
    }
    return result;
}

inline void vEventGroupDelete(EventGroupHandle_t xEventGroup) {
    auto* eg = static_cast<esp32emu::event_groups::EventGroup*>(xEventGroup);
    fprintf(stderr, "[esp32emu] vEventGroupDelete: %p\n", (void*)eg);
    delete eg;
}

// Sync bits (barrier) — simplified mock
inline EventBits_t xEventGroupSync(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToSet,
    const EventBits_t uxBitsToWaitFor,
    TickType_t xTicksToWait)
{
    xEventGroupSetBits(xEventGroup, uxBitsToSet);
    return xEventGroupWaitBits(xEventGroup, uxBitsToWaitFor, pdTRUE, pdTRUE, xTicksToWait);
}
