#include "driver/mcpwm.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    esp32emu::mcpwm::reset();

    // Test GPIO init
    assert(mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_TIMER_0, 15, 16) == ESP_OK);

    // Test init with config
    mcpwm_config_t config{};
    config.frequency = 1000;
    config.cmpr_a = 50.0f;
    config.cmpr_b = 30.0f;
    config.duty_mode = MCPWM_DUTY_MODE_0;
    assert(mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &config) == ESP_OK);

    // Verify state
    auto st = esp32emu::mcpwm::getState(MCPWM_UNIT_0, MCPWM_TIMER_0);
    assert(st.frequency == 1000);
    assert(fabs(st.duty_a - 50.0f) < 0.01f);
    assert(fabs(st.duty_b - 30.0f) < 0.01f);
    assert(st.started == true);
    assert(st.gpio_a == 15);
    assert(st.gpio_b == 16);

    // Test set/get duty
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 75.0f);
    assert(fabs(mcpwm_get_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A) - 75.0f) < 0.01f);

    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, 10.0f);
    assert(fabs(mcpwm_get_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B) - 10.0f) < 0.01f);

    // Test frequency
    mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, 5000);
    assert(mcpwm_get_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0) == 5000);

    // Test start/stop
    mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
    st = esp32emu::mcpwm::getState(MCPWM_UNIT_0, MCPWM_TIMER_0);
    assert(st.started == false);

    mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
    st = esp32emu::mcpwm::getState(MCPWM_UNIT_0, MCPWM_TIMER_0);
    assert(st.started == true);

    // Test multiple units/timers
    mcpwm_config_t config2{};
    config2.frequency = 2000;
    config2.cmpr_a = 25.0f;
    config2.cmpr_b = 75.0f;
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_1, &config2);
    auto st2 = esp32emu::mcpwm::getState(MCPWM_UNIT_1, MCPWM_TIMER_1);
    assert(st2.frequency == 2000);
    assert(fabs(st2.duty_a - 25.0f) < 0.01f);

    // Original timer should be unaffected
    st = esp32emu::mcpwm::getState(MCPWM_UNIT_0, MCPWM_TIMER_0);
    assert(st.frequency == 5000);

    printf("test_mcpwm: all assertions passed\n");
    return 0;
}
