// esp32emu – ESP-IDF event loop mock
#pragma once
#include <cstdint>
#include <cstdio>
#include <functional>
#include <vector>
#include <mutex>
#include <algorithm>
#include <cstring>

#ifndef ESP_OK
#define ESP_OK   0
#define ESP_FAIL -1
#endif
typedef int esp_err_t;

typedef const char* esp_event_base_t;
typedef void* esp_event_loop_handle_t;
typedef void (*esp_event_handler_t)(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data);

#define ESP_EVENT_ANY_BASE nullptr
#define ESP_EVENT_ANY_ID   -1

// Common event bases
#define WIFI_EVENT     "WIFI_EVENT"
#define IP_EVENT       "IP_EVENT"
#define ETH_EVENT      "ETH_EVENT"

// WiFi events
#define WIFI_EVENT_STA_START        0
#define WIFI_EVENT_STA_CONNECTED    1
#define WIFI_EVENT_STA_DISCONNECTED 2
#define WIFI_EVENT_AP_START         3

// IP events
#define IP_EVENT_STA_GOT_IP   0
#define IP_EVENT_STA_LOST_IP  1

struct esp_event_loop_args_t {
    int queue_size;
    const char* task_name;
    int task_priority;
    int task_stack_size;
    int task_core_id;
};

namespace esp32emu {

struct EventHandler {
    esp_event_base_t base;
    int32_t id;
    esp_event_handler_t handler;
    void* arg;
    int regId;
};

class EventLoop {
public:
    static EventLoop& defaultLoop() {
        static EventLoop inst;
        return inst;
    }

    esp_err_t registerHandler(esp_event_base_t base, int32_t id,
                               esp_event_handler_t handler, void* arg) {
        std::lock_guard<std::mutex> lk(mu_);
        handlers_.push_back({base, id, handler, arg, nextId_++});
        return ESP_OK;
    }

    esp_err_t unregisterHandler(esp_event_base_t base, int32_t id,
                                 esp_event_handler_t handler) {
        std::lock_guard<std::mutex> lk(mu_);
        handlers_.erase(
            std::remove_if(handlers_.begin(), handlers_.end(),
                [&](const EventHandler& h) {
                    return h.base == base && h.id == id && h.handler == handler;
                }),
            handlers_.end());
        return ESP_OK;
    }

    esp_err_t post(esp_event_base_t base, int32_t id,
                    const void* data, size_t dataSize, int ticksToWait) {
        (void)ticksToWait; (void)dataSize;
        std::vector<EventHandler> matching;
        {
            std::lock_guard<std::mutex> lk(mu_);
            for (auto& h : handlers_) {
                bool baseMatch = (h.base == ESP_EVENT_ANY_BASE) ||
                                 (base && h.base && strcmp(h.base, base) == 0);
                bool idMatch = (h.id == ESP_EVENT_ANY_ID) || (h.id == id);
                if (baseMatch && idMatch) {
                    matching.push_back(h);
                }
            }
        }
        for (auto& h : matching) {
            h.handler(h.arg, base, id, const_cast<void*>(data));
        }
        return ESP_OK;
    }

    void reset() {
        std::lock_guard<std::mutex> lk(mu_);
        handlers_.clear();
    }

    size_t handlerCount() {
        std::lock_guard<std::mutex> lk(mu_);
        return handlers_.size();
    }

private:
    std::mutex mu_;
    std::vector<EventHandler> handlers_;
    int nextId_ = 1;
};

} // namespace esp32emu

inline esp_err_t esp_event_loop_create_default() {
    return ESP_OK;
}

inline esp_err_t esp_event_loop_delete_default() {
    esp32emu::EventLoop::defaultLoop().reset();
    return ESP_OK;
}

inline esp_err_t esp_event_handler_register(esp_event_base_t base, int32_t id,
                                             esp_event_handler_t handler, void* arg) {
    return esp32emu::EventLoop::defaultLoop().registerHandler(base, id, handler, arg);
}

inline esp_err_t esp_event_handler_unregister(esp_event_base_t base, int32_t id,
                                               esp_event_handler_t handler) {
    return esp32emu::EventLoop::defaultLoop().unregisterHandler(base, id, handler);
}

inline esp_err_t esp_event_post(esp_event_base_t base, int32_t id,
                                 const void* data, size_t dataSize, int ticksToWait) {
    return esp32emu::EventLoop::defaultLoop().post(base, id, data, dataSize, ticksToWait);
}
