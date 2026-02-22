#pragma once
// esp32emu — OneButton library mock

#include <cstdint>
#include <cstdio>
#include <functional>

class OneButton {
public:
    using callback_t = std::function<void()>;

    OneButton() = default;
    OneButton(int pin, bool activeLow = true, bool pullupActive = true)
        : pin_(pin), active_low_(activeLow) {
        (void)pullupActive;
        fprintf(stderr, "[esp32emu] OneButton: pin %d, activeLow=%d\n", pin, activeLow);
    }

    void setup(int pin, bool activeLow = true, bool pullupActive = true) {
        pin_ = pin;
        active_low_ = activeLow;
        (void)pullupActive;
    }

    void attachClick(callback_t cb)       { click_cb_ = cb; }
    void attachDoubleClick(callback_t cb) { double_click_cb_ = cb; }
    void attachLongPressStart(callback_t cb) { long_press_start_cb_ = cb; }
    void attachLongPressStop(callback_t cb)  { long_press_stop_cb_ = cb; }
    void attachDuringLongPress(callback_t cb) { during_long_press_cb_ = cb; }
    void attachMultiClick(callback_t cb)  { multi_click_cb_ = cb; }

    void tick() { ticks_++; }
    void tick(bool level) { ticks_++; level_ = level; }
    void reset() { ticks_ = 0; clicks_ = 0; }

    int getNumberClicks() const { return clicks_; }
    int pin() const { return pin_; }
    bool isIdle() const { return idle_; }
    bool isLongPressed() const { return long_pressed_; }

    void setDebounceMs(uint16_t ms)     { debounce_ms_ = ms; }
    void setClickMs(uint16_t ms)        { click_ms_ = ms; }
    void setPressMs(uint16_t ms)        { press_ms_ = ms; }

    uint16_t getDebounceMs() const { return debounce_ms_; }
    uint16_t getClickMs() const    { return click_ms_; }
    uint16_t getPressMs() const    { return press_ms_; }

    // Test helpers
    void _simulateClick() {
        clicks_ = 1;
        if (click_cb_) click_cb_();
    }
    void _simulateDoubleClick() {
        clicks_ = 2;
        if (double_click_cb_) double_click_cb_();
    }
    void _simulateLongPress() {
        long_pressed_ = true;
        if (long_press_start_cb_) long_press_start_cb_();
    }
    void _simulateLongPressStop() {
        long_pressed_ = false;
        if (long_press_stop_cb_) long_press_stop_cb_();
    }
    void _simulateMultiClick(int n) {
        clicks_ = n;
        if (multi_click_cb_) multi_click_cb_();
    }
    void _setIdle(bool v) { idle_ = v; }

private:
    int pin_ = -1;
    bool active_low_ = true;
    bool level_ = false;
    bool idle_ = true;
    bool long_pressed_ = false;
    int ticks_ = 0;
    int clicks_ = 0;
    uint16_t debounce_ms_ = 50;
    uint16_t click_ms_ = 400;
    uint16_t press_ms_ = 800;
    callback_t click_cb_;
    callback_t double_click_cb_;
    callback_t long_press_start_cb_;
    callback_t long_press_stop_cb_;
    callback_t during_long_press_cb_;
    callback_t multi_click_cb_;
};
