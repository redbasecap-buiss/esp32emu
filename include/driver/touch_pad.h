#pragma once
/*  driver/touch_pad.h — ESP-IDF touch pad driver mock for esp32emu
 *
 *  Emulates the ESP32 capacitive touch sensor driver API.
 *  Supports FSM mode, interrupt thresholds, filtering, and test injection.
 */

#include "esp_err.h"
#include <cstdint>
#include <functional>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

typedef int touch_pad_t;

#define TOUCH_PAD_NUM0   0
#define TOUCH_PAD_NUM1   1
#define TOUCH_PAD_NUM2   2
#define TOUCH_PAD_NUM3   3
#define TOUCH_PAD_NUM4   4
#define TOUCH_PAD_NUM5   5
#define TOUCH_PAD_NUM6   6
#define TOUCH_PAD_NUM7   7
#define TOUCH_PAD_NUM8   8
#define TOUCH_PAD_NUM9   9
#define TOUCH_PAD_MAX    10

typedef enum {
    TOUCH_HVOLT_2V4 = 0,
    TOUCH_HVOLT_2V5,
    TOUCH_HVOLT_2V6,
    TOUCH_HVOLT_2V7,
    TOUCH_HVOLT_MAX,
} touch_high_volt_t;

typedef enum {
    TOUCH_LVOLT_0V5 = 0,
    TOUCH_LVOLT_0V6,
    TOUCH_LVOLT_0V7,
    TOUCH_LVOLT_0V8,
    TOUCH_LVOLT_MAX,
} touch_low_volt_t;

typedef enum {
    TOUCH_HVOLT_ATTEN_1V5 = 0,
    TOUCH_HVOLT_ATTEN_1V,
    TOUCH_HVOLT_ATTEN_0V5,
    TOUCH_HVOLT_ATTEN_0V,
    TOUCH_HVOLT_ATTEN_MAX,
} touch_volt_atten_t;

typedef enum {
    TOUCH_PAD_INTR_DISABLE = 0,
    TOUCH_PAD_INTR_MASK_ACTIVE,
    TOUCH_PAD_INTR_MASK_INACTIVE,
    TOUCH_PAD_INTR_MASK_ALL,
} touch_pad_intr_mask_t;

typedef enum {
    TOUCH_FSM_MODE_TIMER = 0,
    TOUCH_FSM_MODE_SW,
    TOUCH_FSM_MODE_MAX,
} touch_fsm_mode_t;

typedef enum {
    TOUCH_TRIGGER_BELOW = 0,
    TOUCH_TRIGGER_ABOVE,
    TOUCH_TRIGGER_MAX,
} touch_trigger_mode_t;

typedef void (*touch_isr_cb_t)(void* arg);

#ifdef __cplusplus
} // extern "C"
#endif

namespace esp32emu_touch_driver {

struct PadState {
    bool configured = false;
    uint16_t raw_value = 1000;   // untouched default
    uint16_t threshold = 0;
    bool interrupt_enabled = false;
};

struct DriverState {
    bool initialized = false;
    touch_fsm_mode_t fsm_mode = TOUCH_FSM_MODE_TIMER;
    touch_trigger_mode_t trigger_mode = TOUCH_TRIGGER_BELOW;
    touch_isr_cb_t isr_cb = nullptr;
    void* isr_arg = nullptr;
    PadState pads[TOUCH_PAD_MAX];
};

inline DriverState& state() {
    static DriverState s;
    return s;
}

inline void reset() {
    state() = DriverState{};
}

} // namespace esp32emu_touch_driver

#ifdef __cplusplus
extern "C" {
#endif

// ── Driver API ──────────────────────────────────────────────

inline esp_err_t touch_pad_init() {
    esp32emu_touch_driver::reset();
    esp32emu_touch_driver::state().initialized = true;
    return ESP_OK;
}

inline esp_err_t touch_pad_deinit() {
    esp32emu_touch_driver::reset();
    return ESP_OK;
}

inline esp_err_t touch_pad_config(touch_pad_t pad, uint16_t /*threshold_unused*/) {
    if (pad < 0 || pad >= TOUCH_PAD_MAX) return ESP_ERR_INVALID_ARG;
    esp32emu_touch_driver::state().pads[pad].configured = true;
    return ESP_OK;
}

inline esp_err_t touch_pad_read(touch_pad_t pad, uint16_t* value) {
    if (pad < 0 || pad >= TOUCH_PAD_MAX || !value) return ESP_ERR_INVALID_ARG;
    *value = esp32emu_touch_driver::state().pads[pad].raw_value;
    return ESP_OK;
}

inline esp_err_t touch_pad_read_filtered(touch_pad_t pad, uint16_t* value) {
    return touch_pad_read(pad, value);  // same in emulation
}

inline esp_err_t touch_pad_read_raw_data(touch_pad_t pad, uint16_t* value) {
    return touch_pad_read(pad, value);
}

inline esp_err_t touch_pad_set_thresh(touch_pad_t pad, uint16_t threshold) {
    if (pad < 0 || pad >= TOUCH_PAD_MAX) return ESP_ERR_INVALID_ARG;
    esp32emu_touch_driver::state().pads[pad].threshold = threshold;
    return ESP_OK;
}

inline esp_err_t touch_pad_get_thresh(touch_pad_t pad, uint16_t* threshold) {
    if (pad < 0 || pad >= TOUCH_PAD_MAX || !threshold) return ESP_ERR_INVALID_ARG;
    *threshold = esp32emu_touch_driver::state().pads[pad].threshold;
    return ESP_OK;
}

inline esp_err_t touch_pad_set_trigger_mode(touch_trigger_mode_t mode) {
    esp32emu_touch_driver::state().trigger_mode = mode;
    return ESP_OK;
}

inline esp_err_t touch_pad_get_trigger_mode(touch_trigger_mode_t* mode) {
    if (!mode) return ESP_ERR_INVALID_ARG;
    *mode = esp32emu_touch_driver::state().trigger_mode;
    return ESP_OK;
}

inline esp_err_t touch_pad_set_fsm_mode(touch_fsm_mode_t mode) {
    esp32emu_touch_driver::state().fsm_mode = mode;
    return ESP_OK;
}

inline esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t* mode) {
    if (!mode) return ESP_ERR_INVALID_ARG;
    *mode = esp32emu_touch_driver::state().fsm_mode;
    return ESP_OK;
}

inline esp_err_t touch_pad_filter_start(uint32_t /*filter_period_ms*/) {
    return ESP_OK;
}

inline esp_err_t touch_pad_filter_stop() {
    return ESP_OK;
}

inline esp_err_t touch_pad_isr_register(touch_isr_cb_t fn, void* arg) {
    esp32emu_touch_driver::state().isr_cb = fn;
    esp32emu_touch_driver::state().isr_arg = arg;
    return ESP_OK;
}

inline esp_err_t touch_pad_isr_deregister(touch_isr_cb_t /*fn*/, void* /*arg*/) {
    esp32emu_touch_driver::state().isr_cb = nullptr;
    esp32emu_touch_driver::state().isr_arg = nullptr;
    return ESP_OK;
}

inline esp_err_t touch_pad_intr_enable() {
    for (int i = 0; i < TOUCH_PAD_MAX; i++)
        esp32emu_touch_driver::state().pads[i].interrupt_enabled = true;
    return ESP_OK;
}

inline esp_err_t touch_pad_intr_disable() {
    for (int i = 0; i < TOUCH_PAD_MAX; i++)
        esp32emu_touch_driver::state().pads[i].interrupt_enabled = false;
    return ESP_OK;
}

inline esp_err_t touch_pad_set_voltage(touch_high_volt_t /*h*/, touch_low_volt_t /*l*/, touch_volt_atten_t /*a*/) {
    return ESP_OK;
}

#ifdef __cplusplus
}
#endif

// ── Test helpers ────────────────────────────────────────────

namespace esp32emu_touch_driver {

/// Inject a raw reading for a pad; fires ISR if threshold crossed
inline void inject_value(touch_pad_t pad, uint16_t value) {
    if (pad < 0 || pad >= TOUCH_PAD_MAX) return;
    auto& s = state();
    auto& p = s.pads[pad];
    uint16_t old_val = p.raw_value;
    p.raw_value = value;

    if (p.interrupt_enabled && p.threshold > 0 && s.isr_cb) {
        bool fire = false;
        if (s.trigger_mode == TOUCH_TRIGGER_BELOW) {
            fire = (old_val >= p.threshold && value < p.threshold);
        } else {
            fire = (old_val <= p.threshold && value > p.threshold);
        }
        if (fire) s.isr_cb(s.isr_arg);
    }
}

} // namespace esp32emu_touch_driver
