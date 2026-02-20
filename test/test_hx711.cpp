#include "HX711.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    HX711 scale;
    assert(!scale.isBegun());

    scale.begin(4, 5, 128);
    assert(scale.isBegun());
    assert(scale.getDoutPin() == 4);
    assert(scale.getSckPin() == 5);
    assert(scale.get_gain() == 128);
    assert(scale.is_ready());

    // Raw value
    scale.setRawValue(100000);
    assert(scale.read() == 100000);
    assert(scale.read_average(10) == 100000);

    // Tare
    scale.tare();
    assert(scale.get_offset() == 100000);
    assert(fabs(scale.get_value()) < 0.01);

    // Scale factor
    scale.set_scale(420.0f);
    assert(scale.get_scale() == 420.0f);

    // Weight reading
    scale.setRawValue(142000);
    // get_units = (raw - offset) / scale = (142000 - 100000) / 420 = 100.0
    float units = scale.get_units();
    assert(fabs(units - 100.0f) < 0.1f);

    // Manual offset
    scale.set_offset(0);
    assert(scale.get_offset() == 0);

    // Power down/up
    assert(scale.isPowered());
    scale.power_down();
    assert(!scale.isPowered());
    assert(!scale.is_ready());
    scale.power_up();
    assert(scale.isPowered());

    // Wait ready
    assert(scale.wait_ready(100));
    assert(scale.wait_ready_retry(3, 10));
    assert(scale.wait_ready_timeout(100, 10));

    printf("test_hx711: all assertions passed\n");
    return 0;
}
