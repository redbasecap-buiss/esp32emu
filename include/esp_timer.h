#pragma once
// esp32emu — ESP-IDF high-resolution timer mock
// Provides esp_timer_create, start_periodic, start_once, stop, delete, get_time.

#include <cstdint>
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <vector>
#include <cstring>

typedef enum {
    ESP_OK = 0,
    ESP_ERR_INVALID_ARG = 0x102,
    ESP_ERR_INVALID_STATE = 0x103,
    ESP_ERR_NOT_FOUND = 0x105
} esp_timer_err_t;

typedef void (*esp_timer_cb_t)(void* arg);

typedef enum {
    ESP_TIMER_TASK = 0,
    ESP_TIMER_ISR  = 1
} esp_timer_dispatch_t;

typedef struct {
    esp_timer_cb_t callback;
    void* arg;
    esp_timer_dispatch_t dispatch_method;
    const char* name;
    bool skip_unhandled_events;
} esp_timer_create_args_t;

struct esp_timer {
    esp_timer_cb_t callback = nullptr;
    void* arg = nullptr;
    std::string name;
    std::atomic<bool> running{false};
    std::atomic<bool> alive{true};
    std::atomic<bool> periodic{false};
    uint64_t period_us = 0;
    std::thread worker;
    std::mutex mtx;
    int fire_count = 0;
};

typedef esp_timer* esp_timer_handle_t;

namespace esp32emu {
namespace timer {

struct TimerRegistry {
    std::mutex mtx;
    std::vector<esp_timer_handle_t> timers;
    static TimerRegistry& instance() {
        static TimerRegistry r;
        return r;
    }
    void add(esp_timer_handle_t t) {
        std::lock_guard<std::mutex> lk(mtx);
        timers.push_back(t);
    }
    void remove(esp_timer_handle_t t) {
        std::lock_guard<std::mutex> lk(mtx);
        timers.erase(std::remove(timers.begin(), timers.end(), t), timers.end());
    }
};

inline void timer_thread(esp_timer_handle_t t) {
    while (t->alive.load()) {
        if (!t->running.load()) {
            std::this_thread::sleep_for(std::chrono::microseconds(500));
            continue;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(t->period_us));
        if (t->running.load() && t->alive.load()) {
            t->callback(t->arg);
            t->fire_count++;
            if (!t->periodic.load()) {
                t->running.store(false);
            }
        }
    }
}

} // namespace timer
} // namespace esp32emu

inline int esp_timer_create(const esp_timer_create_args_t* args, esp_timer_handle_t* out) {
    if (!args || !out || !args->callback) return ESP_ERR_INVALID_ARG;
    auto* t = new esp_timer();
    t->callback = args->callback;
    t->arg = args->arg;
    t->name = args->name ? args->name : "timer";
    t->worker = std::thread(esp32emu::timer::timer_thread, t);
    esp32emu::timer::TimerRegistry::instance().add(t);
    *out = t;
    return ESP_OK;
}

inline int esp_timer_start_periodic(esp_timer_handle_t t, uint64_t period_us) {
    if (!t) return ESP_ERR_INVALID_ARG;
    t->period_us = period_us;
    t->periodic.store(true);
    t->running.store(true);
    return ESP_OK;
}

inline int esp_timer_start_once(esp_timer_handle_t t, uint64_t timeout_us) {
    if (!t) return ESP_ERR_INVALID_ARG;
    t->period_us = timeout_us;
    t->periodic.store(false);
    t->running.store(true);
    return ESP_OK;
}

inline int esp_timer_stop(esp_timer_handle_t t) {
    if (!t) return ESP_ERR_INVALID_ARG;
    t->running.store(false);
    return ESP_OK;
}

inline int esp_timer_delete(esp_timer_handle_t t) {
    if (!t) return ESP_ERR_INVALID_ARG;
    t->alive.store(false);
    t->running.store(false);
    if (t->worker.joinable()) t->worker.join();
    esp32emu::timer::TimerRegistry::instance().remove(t);
    delete t;
    return ESP_OK;
}

inline int64_t esp_timer_get_time(void) {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
}

inline bool esp_timer_is_active(esp_timer_handle_t t) {
    return t && t->running.load();
}

// Test helpers
namespace esp_timer_test {
    inline int getFireCount(esp_timer_handle_t t) { return t ? t->fire_count : 0; }
    inline void resetFireCount(esp_timer_handle_t t) { if (t) t->fire_count = 0; }
}
