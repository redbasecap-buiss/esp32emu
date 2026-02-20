// Test: RTC IO driver mock
#include "driver/rtc_io.h"
#include <cassert>
#include <cstdio>

int main() {
    esp32emu_rtc_io::reset();

    // Valid RTC pin
    assert(rtc_gpio_is_valid_gpio(4));
    assert(rtc_gpio_is_valid_gpio(25));
    assert(!rtc_gpio_is_valid_gpio(5));  // GPIO5 is not RTC

    // Init
    assert(rtc_gpio_init(4) == ESP_OK);
    assert(rtc_gpio_init(5) == ESP_ERR_INVALID_ARG);

    // Direction
    assert(rtc_gpio_set_direction(4, RTC_GPIO_MODE_OUTPUT_ONLY) == ESP_OK);

    // Level
    assert(rtc_gpio_set_level(4, 1) == ESP_OK);
    assert(rtc_gpio_get_level(4) == 1);
    assert(rtc_gpio_set_level(4, 0) == ESP_OK);
    assert(rtc_gpio_get_level(4) == 0);

    // Pullup/pulldown
    assert(rtc_gpio_pullup_en(4) == ESP_OK);
    assert(esp32emu_rtc_io::states()[4].pullup == true);
    assert(rtc_gpio_pullup_dis(4) == ESP_OK);
    assert(esp32emu_rtc_io::states()[4].pullup == false);

    assert(rtc_gpio_pulldown_en(4) == ESP_OK);
    assert(esp32emu_rtc_io::states()[4].pulldown == true);

    // Hold
    assert(rtc_gpio_hold_en(4) == ESP_OK);
    assert(esp32emu_rtc_io::states()[4].hold == true);
    assert(rtc_gpio_hold_dis(4) == ESP_OK);
    assert(esp32emu_rtc_io::states()[4].hold == false);

    // Deinit
    assert(rtc_gpio_deinit(4) == ESP_OK);

    // Invalid pin operations
    assert(rtc_gpio_set_level(5, 1) == ESP_ERR_INVALID_ARG);
    assert(rtc_gpio_pullup_en(5) == ESP_ERR_INVALID_ARG);

    printf("test_rtc_io: all assertions passed\n");
    return 0;
}
