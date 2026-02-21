#pragma once
// esp32emu — ESP-IDF v5 I2S Standard Mode driver mock
// New-style I2S API: i2s_channel_init_std_mode, i2s_new_channel, etc.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <mutex>

#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif
#ifndef ESP_ERR_INVALID_ARG
#define ESP_ERR_INVALID_ARG 0x102
#endif

typedef void* i2s_chan_handle_t;

typedef enum {
    I2S_DIR_TX = 0,
    I2S_DIR_RX = 1
} i2s_dir_t;

typedef enum {
    I2S_SLOT_MODE_MONO = 1,
    I2S_SLOT_MODE_STEREO = 2
} i2s_slot_mode_t;

typedef enum {
    I2S_DATA_BIT_WIDTH_8BIT  = 8,
    I2S_DATA_BIT_WIDTH_16BIT = 16,
    I2S_DATA_BIT_WIDTH_24BIT = 24,
    I2S_DATA_BIT_WIDTH_32BIT = 32
} i2s_data_bit_width_t;

typedef struct {
    uint32_t sample_rate_hz;
    i2s_data_bit_width_t data_bit_width;
    i2s_slot_mode_t slot_mode;
} i2s_std_clk_config_t;

typedef struct {
    i2s_data_bit_width_t data_bit_width;
    i2s_slot_mode_t slot_mode;
} i2s_std_slot_config_t;

typedef struct {
    int mclk;
    int bclk;
    int ws;
    int dout;
    int din;
} i2s_std_gpio_config_t;

typedef struct {
    i2s_std_clk_config_t clk_cfg;
    i2s_std_slot_config_t slot_cfg;
    i2s_std_gpio_config_t gpio_cfg;
} i2s_std_config_t;

typedef struct {
    int id;
    int dma_desc_num;
    int dma_frame_num;
    bool auto_clear;
} i2s_chan_config_t;

namespace esp32emu {
namespace i2s_std {

struct ChannelState {
    int id = 0;
    i2s_dir_t dir = I2S_DIR_TX;
    bool enabled = false;
    bool configured = false;
    uint32_t sample_rate = 44100;
    i2s_data_bit_width_t bit_width = I2S_DATA_BIT_WIDTH_16BIT;
    i2s_slot_mode_t slot_mode = I2S_SLOT_MODE_STEREO;
    std::vector<uint8_t> tx_buffer;
    std::vector<uint8_t> rx_inject;
    std::mutex mtx;
};

struct I2SStdState {
    ChannelState tx;
    ChannelState rx;
    bool allocated = false;
    std::mutex mtx;

    static I2SStdState& instance() {
        static I2SStdState s;
        return s;
    }
    void reset() {
        std::lock_guard<std::mutex> lk(mtx);
        {
            std::lock_guard<std::mutex> lk2(tx.mtx);
            tx.id = 0; tx.dir = I2S_DIR_TX; tx.enabled = false;
            tx.configured = false; tx.sample_rate = 44100;
            tx.bit_width = I2S_DATA_BIT_WIDTH_16BIT;
            tx.slot_mode = I2S_SLOT_MODE_STEREO;
            tx.tx_buffer.clear(); tx.rx_inject.clear();
        }
        {
            std::lock_guard<std::mutex> lk2(rx.mtx);
            rx.id = 0; rx.dir = I2S_DIR_RX; rx.enabled = false;
            rx.configured = false; rx.sample_rate = 44100;
            rx.bit_width = I2S_DATA_BIT_WIDTH_16BIT;
            rx.slot_mode = I2S_SLOT_MODE_STEREO;
            rx.tx_buffer.clear(); rx.rx_inject.clear();
        }
        allocated = false;
    }
};

} // namespace i2s_std
} // namespace esp32emu

// Opaque handles (just use pointers to the channel states)
inline int i2s_new_channel(const i2s_chan_config_t* cfg,
                            i2s_chan_handle_t* tx_handle,
                            i2s_chan_handle_t* rx_handle) {
    auto& s = esp32emu::i2s_std::I2SStdState::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    s.allocated = true;
    s.tx.id = cfg ? cfg->id : 0;
    s.rx.id = cfg ? cfg->id : 0;
    if (tx_handle) *tx_handle = &s.tx;
    if (rx_handle) *rx_handle = &s.rx;
    printf("[esp32emu] I2S v5: new channel (id=%d)\n", s.tx.id);
    return ESP_OK;
}

inline int i2s_channel_init_std_mode(i2s_chan_handle_t handle, const i2s_std_config_t* cfg) {
    if (!handle || !cfg) return ESP_ERR_INVALID_ARG;
    auto* ch = static_cast<esp32emu::i2s_std::ChannelState*>(handle);
    std::lock_guard<std::mutex> lk(ch->mtx);
    ch->sample_rate = cfg->clk_cfg.sample_rate_hz;
    ch->bit_width = cfg->clk_cfg.data_bit_width;
    ch->slot_mode = cfg->clk_cfg.slot_mode;
    ch->configured = true;
    printf("[esp32emu] I2S v5: std mode init (rate=%u, bits=%d, slots=%d)\n",
           ch->sample_rate, static_cast<int>(ch->bit_width), static_cast<int>(ch->slot_mode));
    return ESP_OK;
}

inline int i2s_channel_enable(i2s_chan_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    auto* ch = static_cast<esp32emu::i2s_std::ChannelState*>(handle);
    std::lock_guard<std::mutex> lk(ch->mtx);
    ch->enabled = true;
    return ESP_OK;
}

inline int i2s_channel_disable(i2s_chan_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    auto* ch = static_cast<esp32emu::i2s_std::ChannelState*>(handle);
    std::lock_guard<std::mutex> lk(ch->mtx);
    ch->enabled = false;
    return ESP_OK;
}

inline int i2s_channel_write(i2s_chan_handle_t handle, const void* src, size_t size,
                              size_t* bytes_written, uint32_t /*timeout_ms*/) {
    if (!handle || !src) return ESP_ERR_INVALID_ARG;
    auto* ch = static_cast<esp32emu::i2s_std::ChannelState*>(handle);
    std::lock_guard<std::mutex> lk(ch->mtx);
    const uint8_t* data = static_cast<const uint8_t*>(src);
    ch->tx_buffer.insert(ch->tx_buffer.end(), data, data + size);
    if (bytes_written) *bytes_written = size;
    return ESP_OK;
}

inline int i2s_channel_read(i2s_chan_handle_t handle, void* dest, size_t size,
                             size_t* bytes_read, uint32_t /*timeout_ms*/) {
    if (!handle || !dest) return ESP_ERR_INVALID_ARG;
    auto* ch = static_cast<esp32emu::i2s_std::ChannelState*>(handle);
    std::lock_guard<std::mutex> lk(ch->mtx);
    size_t avail = std::min(size, ch->rx_inject.size());
    if (avail > 0) {
        memcpy(dest, ch->rx_inject.data(), avail);
        ch->rx_inject.erase(ch->rx_inject.begin(), ch->rx_inject.begin() + avail);
    }
    if (bytes_read) *bytes_read = avail;
    return ESP_OK;
}

inline void i2s_del_channel(i2s_chan_handle_t handle) {
    if (!handle) return;
    auto* ch = static_cast<esp32emu::i2s_std::ChannelState*>(handle);
    std::lock_guard<std::mutex> lk(ch->mtx);
    ch->enabled = false;
    ch->configured = false;
    printf("[esp32emu] I2S v5: channel deleted\n");
}

// Convenience macros matching ESP-IDF
#define I2S_STD_CLK_DEFAULT_CONFIG(rate) \
    { .sample_rate_hz = (rate), .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT, .slot_mode = I2S_SLOT_MODE_STEREO }

#define I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits, mode) \
    { .data_bit_width = (bits), .slot_mode = (mode) }

#define I2S_CHANNEL_DEFAULT_CONFIG(id_val, role) \
    { .id = (id_val), .dma_desc_num = 6, .dma_frame_num = 240, .auto_clear = true }

// Test helpers
namespace esp32emu_i2s_std_test {
    inline std::vector<uint8_t> getTxBuffer() {
        auto& s = esp32emu::i2s_std::I2SStdState::instance();
        std::lock_guard<std::mutex> lk(s.tx.mtx);
        return s.tx.tx_buffer;
    }
    inline void injectRxData(const uint8_t* data, size_t len) {
        auto& s = esp32emu::i2s_std::I2SStdState::instance();
        std::lock_guard<std::mutex> lk(s.rx.mtx);
        s.rx.rx_inject.insert(s.rx.rx_inject.end(), data, data + len);
    }
    inline bool isTxEnabled() {
        auto& s = esp32emu::i2s_std::I2SStdState::instance();
        return s.tx.enabled;
    }
    inline bool isRxEnabled() {
        auto& s = esp32emu::i2s_std::I2SStdState::instance();
        return s.rx.enabled;
    }
    inline uint32_t getSampleRate() {
        auto& s = esp32emu::i2s_std::I2SStdState::instance();
        return s.tx.sample_rate;
    }
    inline void reset() {
        esp32emu::i2s_std::I2SStdState::instance().reset();
    }
}
