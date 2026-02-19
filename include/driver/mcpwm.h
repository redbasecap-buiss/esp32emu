#pragma once
// esp32emu: MCPWM (Motor Control PWM) driver mock
// Supports brushed DC motor control, servo, and capture timer patterns.

#include <cstdint>
#include <cstdio>
#include <functional>
#include <map>
#include <mutex>
#include <string>

// ── Types ──────────────────────────────────────────────────────────
typedef enum { MCPWM_UNIT_0 = 0, MCPWM_UNIT_1 } mcpwm_unit_t;
typedef enum { MCPWM_TIMER_0 = 0, MCPWM_TIMER_1, MCPWM_TIMER_2 } mcpwm_timer_t;
typedef enum { MCPWM_OPR_A = 0, MCPWM_OPR_B } mcpwm_operator_t;
typedef enum { MCPWM_DUTY_MODE_0 = 0, MCPWM_DUTY_MODE_1 } mcpwm_duty_type_t;
typedef enum {
    MCPWM_ACTION_NO_CHANGE = 0,
    MCPWM_ACTION_FORCE_LOW,
    MCPWM_ACTION_FORCE_HIGH,
    MCPWM_ACTION_TOGGLE
} mcpwm_output_action_t;

typedef enum {
    MCPWM_COUNTER_EQUAL_ZERO = 0,
    MCPWM_COUNTER_EQUAL_PERIOD,
    MCPWM_COUNTER_EQUAL_CMPR_A,
    MCPWM_COUNTER_EQUAL_CMPR_B
} mcpwm_counter_event_t;

typedef struct {
    uint32_t frequency;      // Hz
    float cmpr_a;            // duty cycle % for operator A (0-100)
    float cmpr_b;            // duty cycle % for operator B (0-100)
    mcpwm_duty_type_t duty_mode;
    mcpwm_counter_event_t counter_mode; // unused in mock
} mcpwm_config_t;

// ── Internal state ─────────────────────────────────────────────────
namespace esp32emu {
namespace mcpwm {

struct TimerState {
    uint32_t frequency = 0;
    float duty_a = 0.0f;
    float duty_b = 0.0f;
    bool started = false;
    int gpio_a = -1;
    int gpio_b = -1;
};

inline std::mutex& mutex() { static std::mutex m; return m; }
inline std::map<uint32_t, TimerState>& states() {
    static std::map<uint32_t, TimerState> s;
    return s;
}

inline uint32_t key(mcpwm_unit_t unit, mcpwm_timer_t timer) {
    return (uint32_t)unit * 3 + (uint32_t)timer;
}

inline TimerState& get(mcpwm_unit_t unit, mcpwm_timer_t timer) {
    return states()[key(unit, timer)];
}

// Test helpers
inline void reset() {
    std::lock_guard<std::mutex> lk(mutex());
    states().clear();
}

inline TimerState getState(mcpwm_unit_t unit, mcpwm_timer_t timer) {
    std::lock_guard<std::mutex> lk(mutex());
    return get(unit, timer);
}

} // namespace mcpwm
} // namespace esp32emu

// ── API ────────────────────────────────────────────────────────────
typedef int esp_err_t;
#ifndef ESP_OK
#define ESP_OK 0
#endif

inline esp_err_t mcpwm_gpio_init(mcpwm_unit_t unit, mcpwm_timer_t timer,
                                  int gpio_a, int gpio_b) {
    std::lock_guard<std::mutex> lk(esp32emu::mcpwm::mutex());
    auto& st = esp32emu::mcpwm::get(unit, timer);
    st.gpio_a = gpio_a;
    st.gpio_b = gpio_b;
    printf("[mcpwm] unit%d timer%d: GPIO A=%d, B=%d\n",
           (int)unit, (int)timer, gpio_a, gpio_b);
    return ESP_OK;
}

inline esp_err_t mcpwm_init(mcpwm_unit_t unit, mcpwm_timer_t timer,
                             const mcpwm_config_t* config) {
    std::lock_guard<std::mutex> lk(esp32emu::mcpwm::mutex());
    auto& st = esp32emu::mcpwm::get(unit, timer);
    st.frequency = config->frequency;
    st.duty_a = config->cmpr_a;
    st.duty_b = config->cmpr_b;
    st.started = true;
    printf("[mcpwm] unit%d timer%d: freq=%uHz duty_a=%.1f%% duty_b=%.1f%%\n",
           (int)unit, (int)timer, config->frequency, config->cmpr_a, config->cmpr_b);
    return ESP_OK;
}

inline esp_err_t mcpwm_set_duty(mcpwm_unit_t unit, mcpwm_timer_t timer,
                                 mcpwm_operator_t op, float duty) {
    std::lock_guard<std::mutex> lk(esp32emu::mcpwm::mutex());
    auto& st = esp32emu::mcpwm::get(unit, timer);
    if (op == MCPWM_OPR_A) st.duty_a = duty;
    else st.duty_b = duty;
    return ESP_OK;
}

inline float mcpwm_get_duty(mcpwm_unit_t unit, mcpwm_timer_t timer,
                             mcpwm_operator_t op) {
    std::lock_guard<std::mutex> lk(esp32emu::mcpwm::mutex());
    auto& st = esp32emu::mcpwm::get(unit, timer);
    return (op == MCPWM_OPR_A) ? st.duty_a : st.duty_b;
}

inline esp_err_t mcpwm_set_frequency(mcpwm_unit_t unit, mcpwm_timer_t timer,
                                      uint32_t frequency) {
    std::lock_guard<std::mutex> lk(esp32emu::mcpwm::mutex());
    esp32emu::mcpwm::get(unit, timer).frequency = frequency;
    return ESP_OK;
}

inline uint32_t mcpwm_get_frequency(mcpwm_unit_t unit, mcpwm_timer_t timer) {
    std::lock_guard<std::mutex> lk(esp32emu::mcpwm::mutex());
    return esp32emu::mcpwm::get(unit, timer).frequency;
}

inline esp_err_t mcpwm_set_duty_type(mcpwm_unit_t unit, mcpwm_timer_t timer,
                                      mcpwm_operator_t op, mcpwm_duty_type_t duty_type) {
    (void)unit; (void)timer; (void)op; (void)duty_type;
    return ESP_OK;
}

inline esp_err_t mcpwm_start(mcpwm_unit_t unit, mcpwm_timer_t timer) {
    std::lock_guard<std::mutex> lk(esp32emu::mcpwm::mutex());
    esp32emu::mcpwm::get(unit, timer).started = true;
    return ESP_OK;
}

inline esp_err_t mcpwm_stop(mcpwm_unit_t unit, mcpwm_timer_t timer) {
    std::lock_guard<std::mutex> lk(esp32emu::mcpwm::mutex());
    esp32emu::mcpwm::get(unit, timer).started = false;
    return ESP_OK;
}

// Deadtime / sync / capture stubs
inline esp_err_t mcpwm_deadtime_enable(mcpwm_unit_t, mcpwm_timer_t, int, float, float) { return ESP_OK; }
inline esp_err_t mcpwm_deadtime_disable(mcpwm_unit_t, mcpwm_timer_t) { return ESP_OK; }
