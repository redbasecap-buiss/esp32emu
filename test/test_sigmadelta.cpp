#include "driver/sigmadelta.h"
#include <cassert>
#include <cstdio>

int main() {
    esp32emu::sigmadelta::reset();

    // Test config
    sigmadelta_config_t config{};
    config.channel = SIGMADELTA_CHANNEL_0;
    config.sigmadelta_duty = 50;
    config.sigmadelta_prescale = 80;
    config.sigmadelta_gpio = 18;
    assert(sigmadelta_config(&config) == ESP_OK);

    auto st = esp32emu::sigmadelta::getState(SIGMADELTA_CHANNEL_0);
    assert(st.duty == 50);
    assert(st.prescale == 80);
    assert(st.gpio == 18);
    assert(st.configured == true);

    // Test set duty
    assert(sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, -100) == ESP_OK);
    st = esp32emu::sigmadelta::getState(SIGMADELTA_CHANNEL_0);
    assert(st.duty == -100);

    // Test set prescale
    assert(sigmadelta_set_prescale(SIGMADELTA_CHANNEL_0, 200) == ESP_OK);
    st = esp32emu::sigmadelta::getState(SIGMADELTA_CHANNEL_0);
    assert(st.prescale == 200);

    // Test set pin
    assert(sigmadelta_set_pin(SIGMADELTA_CHANNEL_1, 25) == ESP_OK);
    auto st1 = esp32emu::sigmadelta::getState(SIGMADELTA_CHANNEL_1);
    assert(st1.gpio == 25);

    // Channel 0 unchanged
    st = esp32emu::sigmadelta::getState(SIGMADELTA_CHANNEL_0);
    assert(st.gpio == 18);

    // Test negative duty (full range -128..127)
    sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, -128);
    st = esp32emu::sigmadelta::getState(SIGMADELTA_CHANNEL_0);
    assert(st.duty == -128);

    sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, 127);
    st = esp32emu::sigmadelta::getState(SIGMADELTA_CHANNEL_0);
    assert(st.duty == 127);

    printf("test_sigmadelta: all assertions passed\n");
    return 0;
}
