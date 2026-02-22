// esp32emu — OneButton test
#include "OneButton.h"
#include <cassert>
#include <cstdio>

int main() {
    OneButton btn(4, true, true);
    assert(btn.pin() == 4);
    assert(btn.isIdle());

    // Click
    int clicks = 0;
    btn.attachClick([&]() { clicks++; });
    btn._simulateClick();
    assert(clicks == 1);
    assert(btn.getNumberClicks() == 1);

    // Double click
    int dclicks = 0;
    btn.attachDoubleClick([&]() { dclicks++; });
    btn._simulateDoubleClick();
    assert(dclicks == 1);

    // Long press
    int lp_start = 0, lp_stop = 0;
    btn.attachLongPressStart([&]() { lp_start++; });
    btn.attachLongPressStop([&]() { lp_stop++; });
    btn._simulateLongPress();
    assert(lp_start == 1);
    assert(btn.isLongPressed());
    btn._simulateLongPressStop();
    assert(lp_stop == 1);
    assert(!btn.isLongPressed());

    // Multi click
    int multi = 0;
    btn.attachMultiClick([&]() { multi++; });
    btn._simulateMultiClick(3);
    assert(multi == 1);
    assert(btn.getNumberClicks() == 3);

    // Timing config
    btn.setDebounceMs(30);
    btn.setClickMs(300);
    btn.setPressMs(600);
    assert(btn.getDebounceMs() == 30);
    assert(btn.getClickMs() == 300);
    assert(btn.getPressMs() == 600);

    // Tick and reset
    btn.tick();
    btn.tick(true);
    btn.reset();

    // Default constructor
    OneButton btn2;
    btn2.setup(5);
    assert(btn2.pin() == 5);

    printf("test_onebutton: all assertions passed\n");
    return 0;
}
