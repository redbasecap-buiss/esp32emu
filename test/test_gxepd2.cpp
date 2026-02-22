// esp32emu — test_gxepd2: GxEPD2 e-paper display mock test
#include "GxEPD2.h"
#include <cassert>
#include <cstdio>

int main() {
    // 200x200 display
    GxEPD2_BW<GxEPD2_154_D67, 200> display(5, 17, 16, 4);

    assert(!display.test_isInitialized());
    display.init();
    assert(display.test_isInitialized());

    assert(display.width() == 200);
    assert(display.height() == 200);

    // Rotation
    display.setRotation(1);
    assert(display.getRotation() == 1);
    assert(display.width() == 200);  // square, same
    display.setRotation(0);

    // Fill white, check pixel
    display.fillScreen(GxEPD_WHITE);
    assert(display.test_getPixel(0, 0) == 1); // white = 1

    // Draw black pixel
    display.drawPixel(10, 10, GxEPD_BLACK);
    assert(display.test_getPixel(10, 10) == 0); // black = 0
    assert(display.test_getPixel(11, 10) == 1); // neighbor still white

    // Fill rect
    display.fillRect(50, 50, 10, 10, GxEPD_BLACK);
    assert(display.test_getPixel(55, 55) == 0);

    // Full display update
    assert(display.test_getFullUpdates() == 0);
    display.display(false);
    assert(display.test_getFullUpdates() == 1);

    // Partial update
    display.setPartialWindow(0, 0, 50, 50);
    display.display(true);
    assert(display.test_getPartialUpdates() == 1);

    // Paging API
    display.firstPage();
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(10, 20);
    display.setTextColor(GxEPD_BLACK);
    display.setTextSize(2);
    display.println("Hello");
    bool more = display.nextPage();
    assert(!more); // single page in mock
    assert(display.test_getFullUpdates() == 2);

    // 2.13" display
    GxEPD2_BW<GxEPD2_213_B74, 122> display2;
    display2.init();
    assert(display2.width() == 250);
    assert(display2.height() == 122);

    // Hibernate
    display.hibernate();
    assert(display.test_isHibernating());

    printf("test_gxepd2: all assertions passed\n");
    return 0;
}
