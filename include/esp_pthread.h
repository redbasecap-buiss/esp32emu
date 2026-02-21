#pragma once
// esp32emu — esp_pthread mock
// ESP-IDF pthread configuration for stack size, priority, core pinning.

#include <cstdint>
#include <cstdio>

#ifndef ESP_OK
#define ESP_OK 0
#endif

typedef struct {
    size_t stack_size;    // Stack size (bytes), default 3072
    size_t prio;          // Thread priority, default 5
    bool inherit_cfg;     // Inherit config from calling thread
    const char* thread_name; // Thread name
    int pin_to_core;      // Core to pin to (-1 = no pin)
} esp_pthread_cfg_t;

inline esp_pthread_cfg_t esp_pthread_get_default_config() {
    esp_pthread_cfg_t cfg = {};
    cfg.stack_size = 3072;
    cfg.prio = 5;
    cfg.inherit_cfg = false;
    cfg.thread_name = "pthread";
    cfg.pin_to_core = -1;
    return cfg;
}

namespace esp32emu {
namespace pthread_cfg {
    struct State {
        esp_pthread_cfg_t current;
        bool configured = false;
        static State& instance() {
            static State s;
            return s;
        }
    };
} // namespace pthread_cfg
} // namespace esp32emu

inline int esp_pthread_set_cfg(const esp_pthread_cfg_t* cfg) {
    if (!cfg) return -1;
    auto& s = esp32emu::pthread_cfg::State::instance();
    s.current = *cfg;
    s.configured = true;
    printf("[esp32emu] pthread config: stack=%zu prio=%zu core=%d\n",
           cfg->stack_size, cfg->prio, cfg->pin_to_core);
    return ESP_OK;
}

inline int esp_pthread_get_cfg(esp_pthread_cfg_t* cfg) {
    if (!cfg) return -1;
    auto& s = esp32emu::pthread_cfg::State::instance();
    if (!s.configured) {
        *cfg = esp_pthread_get_default_config();
    } else {
        *cfg = s.current;
    }
    return ESP_OK;
}

// Test helpers
namespace esp32emu_pthread_test {
    inline esp_pthread_cfg_t getConfig() {
        auto& s = esp32emu::pthread_cfg::State::instance();
        return s.configured ? s.current : esp_pthread_get_default_config();
    }
    inline bool isConfigured() {
        return esp32emu::pthread_cfg::State::instance().configured;
    }
    inline void reset() {
        auto& s = esp32emu::pthread_cfg::State::instance();
        s.configured = false;
        s.current = {};
    }
}
