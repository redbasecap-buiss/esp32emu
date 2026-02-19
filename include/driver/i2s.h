#pragma once
// esp32emu: ESP-IDF I2S driver mock (legacy API)
#include <cstdint>
#include <cstring>
#include <vector>
#include <mutex>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif

typedef int i2s_port_t;
#define I2S_NUM_0 0
#define I2S_NUM_1 1
#define I2S_NUM_MAX 2

typedef enum {
    I2S_MODE_MASTER = (1 << 0),
    I2S_MODE_SLAVE = (1 << 1),
    I2S_MODE_TX = (1 << 2),
    I2S_MODE_RX = (1 << 3),
    I2S_MODE_DAC_BUILT_IN = (1 << 4),
    I2S_MODE_ADC_BUILT_IN = (1 << 5),
    I2S_MODE_PDM = (1 << 6),
} i2s_mode_t;

typedef enum {
    I2S_COMM_FORMAT_STAND_I2S = 0x01,
    I2S_COMM_FORMAT_STAND_MSB = 0x02,
    I2S_COMM_FORMAT_STAND_PCM_SHORT = 0x04,
    I2S_COMM_FORMAT_STAND_PCM_LONG = 0x0C,
    I2S_COMM_FORMAT_I2S = 0x01,  // legacy alias
} i2s_comm_format_t;

typedef enum {
    I2S_CHANNEL_FMT_RIGHT_LEFT = 0,
    I2S_CHANNEL_FMT_ALL_RIGHT = 1,
    I2S_CHANNEL_FMT_ALL_LEFT = 2,
    I2S_CHANNEL_FMT_ONLY_RIGHT = 3,
    I2S_CHANNEL_FMT_ONLY_LEFT = 4,
} i2s_channel_fmt_t;

typedef enum {
    I2S_BITS_PER_SAMPLE_8BIT = 8,
    I2S_BITS_PER_SAMPLE_16BIT = 16,
    I2S_BITS_PER_SAMPLE_24BIT = 24,
    I2S_BITS_PER_SAMPLE_32BIT = 32,
} i2s_bits_per_sample_t;

typedef struct {
    int mode;   // i2s_mode_t bitmask
    uint32_t sample_rate;
    i2s_bits_per_sample_t bits_per_sample;
    i2s_channel_fmt_t channel_format;
    int communication_format; // i2s_comm_format_t bitmask
    int intr_alloc_flags;
    int dma_buf_count;
    int dma_buf_len;
    bool use_apll;
    bool tx_desc_auto_clear;
    int fixed_mclk;
} i2s_config_t;

typedef struct {
    int bck_io_num;
    int ws_io_num;
    int data_out_num;
    int data_in_num;
} i2s_pin_config_t;

#ifdef __cplusplus
}
#endif

namespace esp32emu {
namespace i2s_driver {

struct I2SPort {
    bool installed = false;
    i2s_config_t config{};
    i2s_pin_config_t pins{};
    std::vector<uint8_t> tx_data;
    std::vector<uint8_t> rx_inject;
    size_t rx_pos = 0;
    std::mutex mtx;

    // Call only while holding mtx or when no contention
    void clear_unlocked() {
        installed = false;
        config = {};
        pins = {};
        tx_data.clear();
        rx_inject.clear();
        rx_pos = 0;
    }
};

inline I2SPort& port(int num) {
    static I2SPort ports[I2S_NUM_MAX];
    return ports[num % I2S_NUM_MAX];
}

inline void reset() {
    for (int i = 0; i < I2S_NUM_MAX; i++) {
        auto& p = port(i);
        std::lock_guard<std::mutex> lk(p.mtx);
        p.clear_unlocked();
    }
}

} // namespace i2s_driver
} // namespace esp32emu

inline int i2s_driver_install(i2s_port_t num, const i2s_config_t* config, int /*queue_size*/, void* /*queue*/) {
    if (num < 0 || num >= I2S_NUM_MAX || !config) return ESP_FAIL;
    auto& p = esp32emu::i2s_driver::port(num);
    std::lock_guard<std::mutex> lk(p.mtx);
    p.installed = true;
    p.config = *config;
    return ESP_OK;
}

inline int i2s_driver_uninstall(i2s_port_t num) {
    if (num < 0 || num >= I2S_NUM_MAX) return ESP_FAIL;
    auto& p = esp32emu::i2s_driver::port(num);
    std::lock_guard<std::mutex> lk(p.mtx);
    p.clear_unlocked();
    return ESP_OK;
}

inline int i2s_set_pin(i2s_port_t num, const i2s_pin_config_t* pin) {
    if (num < 0 || num >= I2S_NUM_MAX) return ESP_FAIL;
    auto& p = esp32emu::i2s_driver::port(num);
    std::lock_guard<std::mutex> lk(p.mtx);
    if (pin) p.pins = *pin;
    return ESP_OK;
}

inline int i2s_set_sample_rates(i2s_port_t num, uint32_t rate) {
    if (num < 0 || num >= I2S_NUM_MAX) return ESP_FAIL;
    auto& p = esp32emu::i2s_driver::port(num);
    std::lock_guard<std::mutex> lk(p.mtx);
    p.config.sample_rate = rate;
    return ESP_OK;
}

inline int i2s_write(i2s_port_t num, const void* src, size_t size, size_t* bytes_written, uint32_t /*ticks*/) {
    if (num < 0 || num >= I2S_NUM_MAX || !src) return ESP_FAIL;
    auto& p = esp32emu::i2s_driver::port(num);
    std::lock_guard<std::mutex> lk(p.mtx);
    const uint8_t* d = (const uint8_t*)src;
    p.tx_data.insert(p.tx_data.end(), d, d + size);
    if (bytes_written) *bytes_written = size;
    return ESP_OK;
}

inline int i2s_read(i2s_port_t num, void* dest, size_t size, size_t* bytes_read, uint32_t /*ticks*/) {
    if (num < 0 || num >= I2S_NUM_MAX || !dest) return ESP_FAIL;
    auto& p = esp32emu::i2s_driver::port(num);
    std::lock_guard<std::mutex> lk(p.mtx);
    size_t avail = p.rx_inject.size() - p.rx_pos;
    size_t n = (size < avail) ? size : avail;
    if (n > 0) memcpy(dest, p.rx_inject.data() + p.rx_pos, n);
    p.rx_pos += n;
    if (bytes_read) *bytes_read = n;
    return ESP_OK;
}

inline int i2s_zero_dma_buffer(i2s_port_t num) {
    if (num < 0 || num >= I2S_NUM_MAX) return ESP_FAIL;
    return ESP_OK;
}

inline int i2s_start(i2s_port_t num) {
    if (num < 0 || num >= I2S_NUM_MAX) return ESP_FAIL;
    return ESP_OK;
}

inline int i2s_stop(i2s_port_t num) {
    if (num < 0 || num >= I2S_NUM_MAX) return ESP_FAIL;
    return ESP_OK;
}
