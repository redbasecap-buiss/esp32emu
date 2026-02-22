// AceButton — Event-Based Button Handling
// Multi-button panel with press, click, double-click, long press events.

#include <Arduino.h>
#include <AceButton.h>

using namespace ace_button;

#define BTN_UP   32
#define BTN_DOWN 33
#define BTN_OK   25

AceButton btnUp(BTN_UP, HIGH, 0);
AceButton btnDown(BTN_DOWN, HIGH, 1);
AceButton btnOk(BTN_OK, HIGH, 2);

int menuIndex = 0;
const char* menuItems[] = {"WiFi Settings", "Display", "Sound", "About"};
const int menuCount = 4;

void handleEvent(AceButton* btn, uint8_t eventType, uint8_t /*state*/) {
    uint8_t id = btn->getId();

    switch (eventType) {
        case kEventClicked:
            if (id == 0) {
                menuIndex = (menuIndex - 1 + menuCount) % menuCount;
                Serial.print("⬆️  Menu: ");
            } else if (id == 1) {
                menuIndex = (menuIndex + 1) % menuCount;
                Serial.print("⬇️  Menu: ");
            } else {
                Serial.print("✅ Selected: ");
            }
            Serial.println(menuItems[menuIndex]);
            break;
        case kEventLongPressed:
            if (id == 2) Serial.println("🔙 Back to main menu");
            break;
        case kEventDoubleClicked:
            if (id == 2) Serial.println("⚙️  Quick settings");
            break;
    }
}

void setup() {
    Serial.begin(115200);

    ButtonConfig* config = ButtonConfig::getSystemButtonConfig();
    config->setEventHandler(handleEvent);
    config->setFeature(kFeatureDoubleClick);
    config->setFeature(kFeatureLongPress);

    Serial.println("🎛️  AceButton Menu Demo");
    Serial.print("▶ ");
    Serial.println(menuItems[menuIndex]);
}

void loop() {
    btnUp.check();
    btnDown.check();
    btnOk.check();
    delay(5);
}
