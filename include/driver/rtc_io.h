// esp32emu – RTC IO driver mock
#pragma once
#include <cstdint>
#include <map>

#ifndef ESP_OK
#define ESP_OK   0
#define ESP_FAIL -1
#endif
#ifndef ESP_ERR_INVALID_ARG
#define ESP_ERR_INVALID_ARG 0x102
#endif
typedef int esp_err_t;
typedef int gpio_num_t;

// RTC GPIO numbers (ESP32 has GPIOs 0,2,4,12-15,25-27,32-39 as RTC)
static const int RTC_GPIO_PINS[] = {0,2,4,12,13,14,15,25,26,27,32,33,34,35,36,37,38,39};

enum rtc_gpio_mode_t {
    RTC_GPIO_MODE_INPUT_ONLY = 0,
    RTC_GPIO_MODE_OUTPUT_ONLY,
    RTC_GPIO_MODE_INPUT_OUTPUT,
    RTC_GPIO_MODE_DISABLED
};

namespace esp32emu_rtc_io {
    struct PinState {
        bool initialized = false;
        rtc_gpio_mode_t mode = RTC_GPIO_MODE_DISABLED;
        int level = 0;
        bool pullup = false;
        bool pulldown = false;
        bool hold = false;
    };

    inline std::map<int, PinState>& states() {
        static std::map<int, PinState> s;
        return s;
    }

    inline bool isRtcPin(int gpio) {
        for (auto p : RTC_GPIO_PINS) if (p == gpio) return true;
        return false;
    }

    inline void reset() { states().clear(); }
}

inline bool rtc_gpio_is_valid_gpio(gpio_num_t gpio) {
    return esp32emu_rtc_io::isRtcPin(gpio);
}

inline esp_err_t rtc_gpio_init(gpio_num_t gpio) {
    if (!esp32emu_rtc_io::isRtcPin(gpio)) return ESP_ERR_INVALID_ARG;
    esp32emu_rtc_io::states()[gpio].initialized = true;
    return ESP_OK;
}

inline esp_err_t rtc_gpio_deinit(gpio_num_t gpio) {
    esp32emu_rtc_io::states().erase(gpio);
    return ESP_OK;
}

inline esp_err_t rtc_gpio_set_direction(gpio_num_t gpio, rtc_gpio_mode_t mode) {
    if (!esp32emu_rtc_io::isRtcPin(gpio)) return ESP_ERR_INVALID_ARG;
    esp32emu_rtc_io::states()[gpio].mode = mode;
    return ESP_OK;
}

inline esp_err_t rtc_gpio_set_level(gpio_num_t gpio, uint32_t level) {
    if (!esp32emu_rtc_io::isRtcPin(gpio)) return ESP_ERR_INVALID_ARG;
    esp32emu_rtc_io::states()[gpio].level = level ? 1 : 0;
    return ESP_OK;
}

inline uint32_t rtc_gpio_get_level(gpio_num_t gpio) {
    auto it = esp32emu_rtc_io::states().find(gpio);
    return (it != esp32emu_rtc_io::states().end()) ? it->second.level : 0;
}

inline esp_err_t rtc_gpio_pullup_en(gpio_num_t gpio) {
    if (!esp32emu_rtc_io::isRtcPin(gpio)) return ESP_ERR_INVALID_ARG;
    esp32emu_rtc_io::states()[gpio].pullup = true;
    return ESP_OK;
}

inline esp_err_t rtc_gpio_pullup_dis(gpio_num_t gpio) {
    if (!esp32emu_rtc_io::isRtcPin(gpio)) return ESP_ERR_INVALID_ARG;
    esp32emu_rtc_io::states()[gpio].pullup = false;
    return ESP_OK;
}

inline esp_err_t rtc_gpio_pulldown_en(gpio_num_t gpio) {
    if (!esp32emu_rtc_io::isRtcPin(gpio)) return ESP_ERR_INVALID_ARG;
    esp32emu_rtc_io::states()[gpio].pulldown = true;
    return ESP_OK;
}

inline esp_err_t rtc_gpio_pulldown_dis(gpio_num_t gpio) {
    if (!esp32emu_rtc_io::isRtcPin(gpio)) return ESP_ERR_INVALID_ARG;
    esp32emu_rtc_io::states()[gpio].pulldown = false;
    return ESP_OK;
}

inline esp_err_t rtc_gpio_hold_en(gpio_num_t gpio) {
    if (!esp32emu_rtc_io::isRtcPin(gpio)) return ESP_ERR_INVALID_ARG;
    esp32emu_rtc_io::states()[gpio].hold = true;
    return ESP_OK;
}

inline esp_err_t rtc_gpio_hold_dis(gpio_num_t gpio) {
    if (!esp32emu_rtc_io::isRtcPin(gpio)) return ESP_ERR_INVALID_ARG;
    esp32emu_rtc_io::states()[gpio].hold = false;
    return ESP_OK;
}

inline esp_err_t rtc_gpio_isolate(gpio_num_t gpio) {
    return rtc_gpio_deinit(gpio);
}
