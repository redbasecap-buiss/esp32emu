#pragma once
// esp32emu: ESP32 TWAI (CAN bus) driver mock
#include <cstdint>
#include <cstring>
#include <vector>
#include <queue>
#include <mutex>

// TWAI mode
typedef enum {
    TWAI_MODE_NORMAL = 0,
    TWAI_MODE_NO_ACK,
    TWAI_MODE_LISTEN_ONLY
} twai_mode_t;

// Timing configs (common baud rates)
typedef struct {
    uint32_t brp;
    uint8_t tseg_1;
    uint8_t tseg_2;
    uint8_t sjw;
    bool triple_sampling;
} twai_timing_config_t;

#define TWAI_TIMING_CONFIG_25KBITS()   {128, 16, 8, 3, false}
#define TWAI_TIMING_CONFIG_50KBITS()   {80, 15, 4, 3, false}
#define TWAI_TIMING_CONFIG_100KBITS()  {40, 15, 4, 3, false}
#define TWAI_TIMING_CONFIG_125KBITS()  {32, 15, 4, 3, false}
#define TWAI_TIMING_CONFIG_250KBITS()  {16, 15, 4, 3, false}
#define TWAI_TIMING_CONFIG_500KBITS()  {8, 15, 4, 3, false}
#define TWAI_TIMING_CONFIG_1MBITS()    {4, 15, 4, 3, false}

// Filter config
typedef struct {
    uint32_t acceptance_code;
    uint32_t acceptance_mask;
    bool single_filter;
} twai_filter_config_t;

#define TWAI_FILTER_CONFIG_ACCEPT_ALL() {0, 0xFFFFFFFF, true}

// General config
typedef struct {
    twai_mode_t mode;
    int tx_io;
    int rx_io;
    int clkout_io;
    int bus_off_io;
    uint32_t tx_queue_len;
    uint32_t rx_queue_len;
    uint32_t alerts_enabled;
    uint32_t clkout_divider;
    int intr_flags;
} twai_general_config_t;

#define TWAI_GENERAL_CONFIG_DEFAULT(tx, rx, m) { \
    .mode = (m), .tx_io = (tx), .rx_io = (rx), \
    .clkout_io = -1, .bus_off_io = -1, \
    .tx_queue_len = 5, .rx_queue_len = 5, \
    .alerts_enabled = 0, .clkout_divider = 0, .intr_flags = 0 }

// CAN message
typedef struct {
    uint32_t identifier;
    uint8_t data_length_code;
    uint8_t data[8];
    struct {
        uint32_t extd : 1;       // Extended frame (29-bit ID)
        uint32_t rtr : 1;        // Remote transmission request
        uint32_t ss : 1;         // Single shot
        uint32_t self : 1;       // Self-reception request
        uint32_t dlc_non_comp : 1;
        uint32_t reserved : 27;
    } flags;
} twai_message_t;

// Status
typedef struct {
    uint32_t state;
    uint32_t msgs_to_tx;
    uint32_t msgs_to_rx;
    uint32_t tx_error_counter;
    uint32_t rx_error_counter;
    uint32_t tx_failed_count;
    uint32_t rx_missed_count;
    uint32_t arb_lost_count;
    uint32_t bus_error_count;
} twai_status_info_t;

typedef int esp_err_t;
#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif
#define ESP_ERR_INVALID_STATE -2
#define ESP_ERR_TIMEOUT -3

namespace _twai_mock {
    struct State {
        bool installed = false;
        bool started = false;
        twai_mode_t mode = TWAI_MODE_NORMAL;
        int tx_pin = -1;
        int rx_pin = -1;
        std::vector<twai_message_t> txLog;
        std::queue<twai_message_t> rxQueue;
        uint32_t txCount = 0;
        uint32_t rxCount = 0;
        std::mutex mtx;
    };
    inline State& state() { static State s; return s; }
}

inline esp_err_t twai_driver_install(const twai_general_config_t* g_config,
                                      const twai_timing_config_t*,
                                      const twai_filter_config_t*) {
    auto& s = _twai_mock::state();
    if (s.installed) return ESP_ERR_INVALID_STATE;
    s.installed = true;
    s.mode = g_config->mode;
    s.tx_pin = g_config->tx_io;
    s.rx_pin = g_config->rx_io;
    return ESP_OK;
}

inline esp_err_t twai_driver_uninstall() {
    auto& s = _twai_mock::state();
    if (!s.installed) return ESP_ERR_INVALID_STATE;
    s.installed = false;
    s.started = false;
    s.txLog.clear();
    while (!s.rxQueue.empty()) s.rxQueue.pop();
    s.txCount = 0;
    s.rxCount = 0;
    return ESP_OK;
}

inline esp_err_t twai_start() {
    auto& s = _twai_mock::state();
    if (!s.installed || s.started) return ESP_ERR_INVALID_STATE;
    s.started = true;
    return ESP_OK;
}

inline esp_err_t twai_stop() {
    auto& s = _twai_mock::state();
    if (!s.started) return ESP_ERR_INVALID_STATE;
    s.started = false;
    return ESP_OK;
}

inline esp_err_t twai_transmit(const twai_message_t* msg, uint32_t) {
    auto& s = _twai_mock::state();
    if (!s.started) return ESP_ERR_INVALID_STATE;
    std::lock_guard<std::mutex> lk(s.mtx);
    s.txLog.push_back(*msg);
    s.txCount++;
    return ESP_OK;
}

inline esp_err_t twai_receive(twai_message_t* msg, uint32_t) {
    auto& s = _twai_mock::state();
    if (!s.started) return ESP_ERR_INVALID_STATE;
    std::lock_guard<std::mutex> lk(s.mtx);
    if (s.rxQueue.empty()) return ESP_ERR_TIMEOUT;
    *msg = s.rxQueue.front();
    s.rxQueue.pop();
    s.rxCount++;
    return ESP_OK;
}

inline esp_err_t twai_get_status_info(twai_status_info_t* status) {
    auto& s = _twai_mock::state();
    memset(status, 0, sizeof(*status));
    status->state = s.started ? 1 : 0;
    std::lock_guard<std::mutex> lk(s.mtx);
    status->msgs_to_rx = s.rxQueue.size();
    return ESP_OK;
}

// Test helpers
namespace twai_test {
    inline void injectRxMessage(uint32_t id, const uint8_t* data, uint8_t len, bool extd = false) {
        auto& s = _twai_mock::state();
        twai_message_t msg;
        memset(&msg, 0, sizeof(msg));
        msg.identifier = id;
        msg.data_length_code = len;
        if (data && len > 0) memcpy(msg.data, data, std::min((int)len, 8));
        msg.flags.extd = extd ? 1 : 0;
        std::lock_guard<std::mutex> lk(s.mtx);
        s.rxQueue.push(msg);
    }
    inline const std::vector<twai_message_t>& getTxLog() { return _twai_mock::state().txLog; }
    inline uint32_t getTxCount() { return _twai_mock::state().txCount; }
    inline uint32_t getRxCount() { return _twai_mock::state().rxCount; }
    inline void reset() { twai_driver_uninstall(); }
}
