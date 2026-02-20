// Test: soc/ header stubs compile and provide expected values
#include "soc/soc.h"
#include "soc/gpio_num.h"
#include "soc/rtc.h"
#include "soc/timer_group_struct.h"
#include <cassert>
#include <cstdio>

int main() {
    // GPIO num enum
    assert(GPIO_NUM_NC == -1);
    assert(GPIO_NUM_0 == 0);
    assert(GPIO_NUM_39 == 39);
    assert(GPIO_NUM_MAX == 40);

    // SoC defines
    assert(SOC_GPIO_PIN_COUNT == 40);
    assert(SOC_ADC_CHANNEL_NUM == 10);

    // REG macros compile
    (void)REG_READ(0x1000);
    REG_WRITE(0x1000, 42);
    REG_SET_BIT(0x1000, 1);
    REG_CLR_BIT(0x1000, 1);

    // RTC enums
    assert(RTC_CPU_FREQ_240M == 3);
    assert(RTC_SLOW_FREQ_RTC == 0);
    assert(RTC_FAST_FREQ_8M == 1);

    // Timer group struct
    TIMERG0.hw_timer[0].config.val = 123;
    assert(TIMERG0.hw_timer[0].config.val == 123);

    printf("test_soc: all assertions passed\n");
    return 0;
}
