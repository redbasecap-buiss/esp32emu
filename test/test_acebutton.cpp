// esp32emu — AceButton test
#include "Arduino.h"
#include "AceButton.h"
#include <cassert>
#include <cstdio>

int main() {
    using namespace ace_button;

    // Basic construction
    AceButton btn(4, HIGH, 0);
    assert(btn.getPin() == 4);
    assert(btn.getId() == 0);

    // Config
    ButtonConfig config;
    config.setFeature(kFeatureDoubleClick);
    config.setFeature(kFeatureLongPress);
    assert(config.isFeature(kFeatureClick));
    assert(config.isFeature(kFeatureDoubleClick));
    config.clearFeature(kFeatureDoubleClick);
    assert(!config.isFeature(kFeatureDoubleClick));

    // Timing
    config.setDebounceDelay(30);
    config.setClickDelay(250);
    config.setLongPressDelay(800);
    assert(config.getDebounceDelay() == 30);
    assert(config.getClickDelay() == 250);
    assert(config.getLongPressDelay() == 800);

    // Event handler
    int press_count = 0, click_count = 0, long_count = 0;
    uint8_t last_event = 255;
    config.setEventHandler([&](AceButton* b, uint8_t eventType, uint8_t /*state*/) {
        last_event = eventType;
        if (eventType == kEventPressed) press_count++;
        if (eventType == kEventClicked) click_count++;
        if (eventType == kEventLongPressed) long_count++;
        (void)b;
    });

    btn.setButtonConfig(&config);
    assert(btn.getButtonConfig() == &config);

    // Simulate events
    btn._simulateEvent(kEventPressed, LOW);
    assert(press_count == 1);
    assert(last_event == kEventPressed);
    assert(btn.getLastButtonState() == LOW);

    btn._simulateEvent(kEventClicked);
    assert(click_count == 1);

    btn._simulateEvent(kEventLongPressed);
    assert(long_count == 1);

    // Check tick
    btn.check();
    btn.check();
    assert(btn._getCheckCount() == 2);

    // System config
    auto* sys = ButtonConfig::getSystemButtonConfig();
    assert(sys != nullptr);

    // Init method
    AceButton btn2;
    btn2.init(7, HIGH, 3);
    assert(btn2.getPin() == 7);
    assert(btn2.getId() == 3);

    // Without config, uses system
    assert(btn2.getButtonConfig() == sys);

    printf("test_acebutton: all assertions passed\n");
    return 0;
}
