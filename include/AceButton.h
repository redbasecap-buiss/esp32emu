#pragma once
// esp32emu — AceButton library mock

#include <cstdint>
#include <cstdio>
#include <functional>

namespace ace_button {

class AceButton;

using ButtonEventHandler = std::function<void(AceButton*, uint8_t, uint8_t)>;

constexpr uint8_t kEventPressed      = 0;
constexpr uint8_t kEventReleased     = 1;
constexpr uint8_t kEventClicked      = 2;
constexpr uint8_t kEventDoubleClicked = 3;
constexpr uint8_t kEventLongPressed  = 4;
constexpr uint8_t kEventRepeatPressed = 5;

constexpr uint8_t kFeatureClick       = 0x01;
constexpr uint8_t kFeatureDoubleClick = 0x02;
constexpr uint8_t kFeatureLongPress   = 0x04;
constexpr uint8_t kFeatureRepeatPress = 0x08;
constexpr uint8_t kFeatureSuppressAfterClick      = 0x10;
constexpr uint8_t kFeatureSuppressAfterDoubleClick = 0x20;
constexpr uint8_t kFeatureSuppressAfterLongPress   = 0x40;
constexpr uint8_t kFeatureSuppressAll              = 0x80;

class ButtonConfig {
public:
    ButtonConfig() = default;

    void setEventHandler(ButtonEventHandler handler) { handler_ = handler; }
    ButtonEventHandler getEventHandler() const { return handler_; }

    void setFeature(uint8_t f)  { features_ |= f; }
    void clearFeature(uint8_t f) { features_ &= ~f; }
    bool isFeature(uint8_t f) const { return features_ & f; }

    void setDebounceDelay(uint16_t ms)   { debounce_ = ms; }
    void setClickDelay(uint16_t ms)      { click_ = ms; }
    void setDoubleClickDelay(uint16_t ms) { dclick_ = ms; }
    void setLongPressDelay(uint16_t ms)   { lpress_ = ms; }
    void setRepeatPressDelay(uint16_t ms) { rpress_ = ms; }
    void setRepeatPressInterval(uint16_t ms) { rinterval_ = ms; }

    uint16_t getDebounceDelay() const   { return debounce_; }
    uint16_t getClickDelay() const      { return click_; }
    uint16_t getLongPressDelay() const   { return lpress_; }

    static ButtonConfig* getSystemButtonConfig() {
        static ButtonConfig cfg;
        return &cfg;
    }

private:
    ButtonEventHandler handler_;
    uint8_t features_ = kFeatureClick;
    uint16_t debounce_ = 20, click_ = 200, dclick_ = 400, lpress_ = 1000, rpress_ = 1000, rinterval_ = 200;
};

class AceButton {
public:
    AceButton() = default;
    explicit AceButton(uint8_t pin, uint8_t defaultState = 1, uint8_t id = 0)
        : pin_(pin), default_state_(defaultState), id_(id) {
        fprintf(stderr, "[esp32emu] AceButton: pin %d, id %d\n", pin, id);
    }

    void init(uint8_t pin, uint8_t defaultState = 1, uint8_t id = 0) {
        pin_ = pin; default_state_ = defaultState; id_ = id;
    }

    void setButtonConfig(ButtonConfig* config) { config_ = config; }
    ButtonConfig* getButtonConfig() const { return config_ ? config_ : ButtonConfig::getSystemButtonConfig(); }

    void check() { checks_++; }
    uint8_t getPin() const { return pin_; }
    uint8_t getId() const { return id_; }
    uint8_t getLastButtonState() const { return last_state_; }

    // Test helpers
    void _simulateEvent(uint8_t eventType, uint8_t buttonState = 0) {
        last_state_ = buttonState;
        auto cfg = getButtonConfig();
        if (cfg && cfg->getEventHandler()) {
            cfg->getEventHandler()(this, eventType, buttonState);
        }
    }
    int _getCheckCount() const { return checks_; }

private:
    uint8_t pin_ = 0;
    uint8_t default_state_ = 1;
    uint8_t id_ = 0;
    uint8_t last_state_ = 1;
    ButtonConfig* config_ = nullptr;
    int checks_ = 0;
};

} // namespace ace_button

using namespace ace_button;
