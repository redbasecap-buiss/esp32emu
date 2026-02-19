#pragma once
// esp32emu — ESP-IDF logging macros mock
// Provides ESP_LOGE/W/I/D/V with colored terminal output and log level filtering.

#include <cstdio>
#include <cstdarg>
#include <cstdint>
#include <string>
#include <functional>

typedef enum {
    ESP_LOG_NONE   = 0,
    ESP_LOG_ERROR  = 1,
    ESP_LOG_WARN   = 2,
    ESP_LOG_INFO   = 3,
    ESP_LOG_DEBUG  = 4,
    ESP_LOG_VERBOSE= 5
} esp_log_level_t;

namespace esp32emu {
namespace log {

struct LogState {
    esp_log_level_t level = ESP_LOG_INFO;
    std::function<void(esp_log_level_t, const char* tag, const char* msg)> callback;
    int counts[6] = {0,0,0,0,0,0};

    static LogState& instance() {
        static LogState s;
        return s;
    }
    void reset() {
        level = ESP_LOG_INFO;
        callback = nullptr;
        for (int i = 0; i < 6; i++) counts[i] = 0;
    }
};

inline void log_printf(esp_log_level_t lvl, const char* tag, const char* fmt, ...) {
    auto& s = LogState::instance();
    if (lvl > s.level) return;
    s.counts[(int)lvl]++;

    char buf[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (s.callback) {
        s.callback(lvl, tag, buf);
        return;
    }

    const char* colors[] = {"", "\033[31m", "\033[33m", "\033[32m", "\033[36m", "\033[90m"};
    const char* labels[] = {"", "E", "W", "I", "D", "V"};
    fprintf(stderr, "%s[%s] %s: %s\033[0m\n", colors[(int)lvl], labels[(int)lvl], tag, buf);
}

} // namespace log
} // namespace esp32emu

inline void esp_log_level_set(const char* tag, esp_log_level_t level) {
    (void)tag;
    esp32emu::log::LogState::instance().level = level;
}

#define ESP_LOGE(tag, fmt, ...) esp32emu::log::log_printf(ESP_LOG_ERROR,   tag, fmt, ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) esp32emu::log::log_printf(ESP_LOG_WARN,    tag, fmt, ##__VA_ARGS__)
#define ESP_LOGI(tag, fmt, ...) esp32emu::log::log_printf(ESP_LOG_INFO,    tag, fmt, ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) esp32emu::log::log_printf(ESP_LOG_DEBUG,   tag, fmt, ##__VA_ARGS__)
#define ESP_LOGV(tag, fmt, ...) esp32emu::log::log_printf(ESP_LOG_VERBOSE, tag, fmt, ##__VA_ARGS__)

// Test helpers
namespace esp_log_test {
    inline void reset() { esp32emu::log::LogState::instance().reset(); }
    inline void setLevel(esp_log_level_t l) { esp32emu::log::LogState::instance().level = l; }
    inline void setCallback(std::function<void(esp_log_level_t, const char*, const char*)> cb) {
        esp32emu::log::LogState::instance().callback = cb;
    }
    inline int getCount(esp_log_level_t l) { return esp32emu::log::LogState::instance().counts[(int)l]; }
}
