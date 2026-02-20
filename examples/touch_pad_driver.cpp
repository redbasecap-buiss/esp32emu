// touch_pad_driver.cpp — ESP-IDF touch pad driver example
// Demonstrates configuring touch pads with the driver API, thresholds, and ISR

#include <Arduino.h>
#include "driver/touch_pad.h"

static volatile bool touched = false;

static void touch_isr(void* /*arg*/) {
    touched = true;
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF Touch Pad Driver Example");

    touch_pad_init();
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V5);
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);

    // Configure pads 0-3
    for (int i = 0; i < 4; i++) {
        touch_pad_config((touch_pad_t)i, 0);
    }

    // Start filter
    touch_pad_filter_start(10);

    // Read initial values and set thresholds at 2/3
    for (int i = 0; i < 4; i++) {
        uint16_t val;
        touch_pad_read_filtered((touch_pad_t)i, &val);
        touch_pad_set_thresh((touch_pad_t)i, val * 2 / 3);
        Serial.printf("  Pad %d: value=%u, threshold=%u\n", i, val, val * 2 / 3);
    }

    // Register ISR
    touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);
    touch_pad_isr_register(touch_isr, nullptr);
    touch_pad_intr_enable();

    Serial.println("Touch pads ready. Inject values to simulate touch.");
}

void loop() {
    if (touched) {
        touched = false;
        Serial.println("👆 Touch detected!");

        // Read all pads
        for (int i = 0; i < 4; i++) {
            uint16_t val;
            touch_pad_read_filtered((touch_pad_t)i, &val);
            uint16_t thresh;
            touch_pad_get_thresh((touch_pad_t)i, &thresh);
            Serial.printf("  Pad %d: %u %s\n", i, val, val < thresh ? "(TOUCHED)" : "");
        }
    }
    delay(100);
}
