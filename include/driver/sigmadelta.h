#pragma once
// esp32emu: Sigma-Delta modulation driver mock

#include <cstdint>
#include <cstdio>
#include <map>
#include <mutex>

#ifndef ESP_OK
#define ESP_OK 0
#endif

typedef int esp_err_t;

typedef enum {
    SIGMADELTA_CHANNEL_0 = 0,
    SIGMADELTA_CHANNEL_1,
    SIGMADELTA_CHANNEL_2,
    SIGMADELTA_CHANNEL_3,
    SIGMADELTA_CHANNEL_4,
    SIGMADELTA_CHANNEL_5,
    SIGMADELTA_CHANNEL_6,
    SIGMADELTA_CHANNEL_7,
    SIGMADELTA_CHANNEL_MAX
} sigmadelta_channel_t;

typedef struct {
    sigmadelta_channel_t channel;
    int8_t sigmadelta_duty;       // -128 to 127
    uint8_t sigmadelta_prescale;  // 0..255
    int sigmadelta_gpio;
} sigmadelta_config_t;

namespace esp32emu {
namespace sigmadelta {

struct ChannelState {
    int8_t duty = 0;
    uint8_t prescale = 0;
    int gpio = -1;
    bool configured = false;
};

inline std::mutex& mutex() { static std::mutex m; return m; }
inline std::map<int, ChannelState>& channels() {
    static std::map<int, ChannelState> c;
    return c;
}

inline void reset() {
    std::lock_guard<std::mutex> lk(mutex());
    channels().clear();
}

inline ChannelState getState(sigmadelta_channel_t ch) {
    std::lock_guard<std::mutex> lk(mutex());
    return channels()[(int)ch];
}

} // namespace sigmadelta
} // namespace esp32emu

inline esp_err_t sigmadelta_config(const sigmadelta_config_t* config) {
    if (!config) return -1;
    std::lock_guard<std::mutex> lk(esp32emu::sigmadelta::mutex());
    auto& st = esp32emu::sigmadelta::channels()[(int)config->channel];
    st.duty = config->sigmadelta_duty;
    st.prescale = config->sigmadelta_prescale;
    st.gpio = config->sigmadelta_gpio;
    st.configured = true;
    printf("[sigmadelta] ch%d: duty=%d prescale=%u gpio=%d\n",
           (int)config->channel, (int)st.duty, (unsigned)st.prescale, st.gpio);
    return ESP_OK;
}

inline esp_err_t sigmadelta_set_duty(sigmadelta_channel_t channel, int8_t duty) {
    std::lock_guard<std::mutex> lk(esp32emu::sigmadelta::mutex());
    esp32emu::sigmadelta::channels()[(int)channel].duty = duty;
    return ESP_OK;
}

inline esp_err_t sigmadelta_set_prescale(sigmadelta_channel_t channel, uint8_t prescale) {
    std::lock_guard<std::mutex> lk(esp32emu::sigmadelta::mutex());
    esp32emu::sigmadelta::channels()[(int)channel].prescale = prescale;
    return ESP_OK;
}

inline esp_err_t sigmadelta_set_pin(sigmadelta_channel_t channel, int gpio_num) {
    std::lock_guard<std::mutex> lk(esp32emu::sigmadelta::mutex());
    esp32emu::sigmadelta::channels()[(int)channel].gpio = gpio_num;
    return ESP_OK;
}
