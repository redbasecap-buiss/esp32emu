#include "RunningMedian.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    RunningMedian rm(5);

    assert(rm.getSize() == 5);
    assert(rm.getCount() == 0);
    assert(!rm.isFull());

    // Add values
    rm.add(10.0f);
    rm.add(20.0f);
    rm.add(30.0f);
    assert(rm.getCount() == 3);
    assert(rm.getMedian() == 20.0f);

    rm.add(5.0f);
    rm.add(15.0f);
    assert(rm.isFull());

    // Median of {10, 20, 30, 5, 15} sorted={5, 10, 15, 20, 30} → 15
    assert(rm.getMedian() == 15.0f);
    assert(rm.getLowest() == 5.0f);
    assert(rm.getHighest() == 30.0f);

    // Average = (10+20+30+5+15)/5 = 16
    assert(fabs(rm.getAverage() - 16.0f) < 0.01f);

    // Overflow: adding 6th pushes out first (10)
    rm.add(25.0f);
    assert(rm.getCount() == 5);
    // Values: {20, 30, 5, 15, 25} sorted={5, 15, 20, 25, 30} → 20
    assert(rm.getMedian() == 20.0f);

    // Element access
    assert(rm.getElement(0) == 20.0f);
    assert(rm.getSortedElement(0) == 5.0f);

    // Clear
    rm.clear();
    assert(rm.getCount() == 0);

    printf("test_runningmedian: all assertions passed\n");
    return 0;
}
