// test_touch_driver.cpp — Tests for driver/touch_pad.h
#include "driver/touch_pad.h"
#include <cassert>
#include <cstdio>

static int isr_count = 0;
static void test_isr(void* arg) {
    (void)arg;
    isr_count++;
}

int main() {
    // Init / deinit
    assert(touch_pad_init() == ESP_OK);
    assert(esp32emu_touch_driver::state().initialized);
    
    // Configure pads
    assert(touch_pad_config(TOUCH_PAD_NUM0, 0) == ESP_OK);
    assert(touch_pad_config(TOUCH_PAD_NUM5, 0) == ESP_OK);
    assert(touch_pad_config(TOUCH_PAD_MAX, 0) == ESP_ERR_INVALID_ARG);  // out of range
    
    // Read default value
    uint16_t val = 0;
    assert(touch_pad_read(TOUCH_PAD_NUM0, &val) == ESP_OK);
    assert(val == 1000);  // default untouched
    
    // Inject value
    esp32emu_touch_driver::inject_value(TOUCH_PAD_NUM0, 300);
    assert(touch_pad_read(TOUCH_PAD_NUM0, &val) == ESP_OK);
    assert(val == 300);
    
    // Filtered read returns same
    assert(touch_pad_read_filtered(TOUCH_PAD_NUM0, &val) == ESP_OK);
    assert(val == 300);
    
    // Threshold + ISR
    assert(touch_pad_set_thresh(TOUCH_PAD_NUM0, 500) == ESP_OK);
    uint16_t thresh = 0;
    assert(touch_pad_get_thresh(TOUCH_PAD_NUM0, &thresh) == ESP_OK);
    assert(thresh == 500);
    
    isr_count = 0;
    assert(touch_pad_isr_register(test_isr, nullptr) == ESP_OK);
    assert(touch_pad_intr_enable() == ESP_OK);
    
    // Set trigger below (default) — ISR fires when crossing below threshold
    esp32emu_touch_driver::inject_value(TOUCH_PAD_NUM0, 600);  // above threshold
    esp32emu_touch_driver::inject_value(TOUCH_PAD_NUM0, 400);  // crosses below → fire
    assert(isr_count == 1);
    
    // No fire when already below
    esp32emu_touch_driver::inject_value(TOUCH_PAD_NUM0, 200);
    assert(isr_count == 1);
    
    // FSM mode
    assert(touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW) == ESP_OK);
    touch_fsm_mode_t fsm;
    assert(touch_pad_get_fsm_mode(&fsm) == ESP_OK);
    assert(fsm == TOUCH_FSM_MODE_SW);
    
    // Trigger mode
    assert(touch_pad_set_trigger_mode(TOUCH_TRIGGER_ABOVE) == ESP_OK);
    touch_trigger_mode_t trig;
    assert(touch_pad_get_trigger_mode(&trig) == ESP_OK);
    assert(trig == TOUCH_TRIGGER_ABOVE);
    
    // Test above trigger: fire when crossing above
    isr_count = 0;
    esp32emu_touch_driver::inject_value(TOUCH_PAD_NUM0, 400);  // below
    esp32emu_touch_driver::inject_value(TOUCH_PAD_NUM0, 600);  // crosses above → fire
    assert(isr_count == 1);
    
    // Disable interrupts
    assert(touch_pad_intr_disable() == ESP_OK);
    esp32emu_touch_driver::inject_value(TOUCH_PAD_NUM0, 300);
    esp32emu_touch_driver::inject_value(TOUCH_PAD_NUM0, 700);
    assert(isr_count == 1);  // no more fires
    
    // Filter start/stop (no-ops, shouldn't fail)
    assert(touch_pad_filter_start(10) == ESP_OK);
    assert(touch_pad_filter_stop() == ESP_OK);
    
    // Voltage config (no-op)
    assert(touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V5) == ESP_OK);
    
    // Deinit resets everything
    assert(touch_pad_deinit() == ESP_OK);
    assert(!esp32emu_touch_driver::state().initialized);
    
    printf("test_touch_driver: all assertions passed\n");
    return 0;
}
