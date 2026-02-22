// test_max17048: MAX17048 battery fuel gauge mock tests
#include "MAX17048.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    MAX17048 gauge;

    // Begin
    assert(gauge.begin());
    assert(gauge.isConnected());

    // Default values
    assert(fabs(gauge.cellVoltage() - 3.7f) < 0.01f);
    assert(fabs(gauge.cellPercent() - 75.0f) < 0.01f);
    assert(fabs(gauge.chargeRate()) < 0.01f);

    // Version
    assert(gauge.getVersion() == 0x0013);

    // Test injection
    gauge.setTestVoltage(4.2f);
    assert(fabs(gauge.cellVoltage() - 4.2f) < 0.01f);
    gauge.setTestSOC(100.0f);
    assert(fabs(gauge.cellPercent() - 100.0f) < 0.01f);
    gauge.setTestRate(-5.0f);
    assert(fabs(gauge.chargeRate() - (-5.0f)) < 0.01f);

    // Alert threshold
    gauge.setAlertThreshold(10);
    assert(gauge.getAlertThreshold() == 10);
    gauge.setAlertThreshold(50); // clamped to 32
    assert(gauge.getAlertThreshold() == 32);
    gauge.setAlertThreshold(0); // clamped to 1
    assert(gauge.getAlertThreshold() == 1);

    // Alert flag
    gauge.setAlertThreshold(20);
    gauge.setTestSOC(15.0f); // below threshold
    assert(gauge.isActiveAlert());
    gauge.setTestSOC(50.0f);
    gauge.clearAlert();
    assert(!gauge.isActiveAlert());
    gauge.setTestAlert(true);
    assert(gauge.isActiveAlert());

    // Hibernate
    assert(!gauge.isHibernating());
    gauge.enableHibernate();
    assert(gauge.isHibernating());
    gauge.disableHibernate();
    assert(!gauge.isHibernating());

    // Sleep
    assert(!gauge.isSleeping());
    gauge.enableSleep();
    assert(gauge.isSleeping());
    gauge.disableSleep();
    assert(!gauge.isSleeping());

    // Reset
    assert(!gauge.isReset());
    gauge.reset();
    assert(gauge.isReset());
    gauge.clearReset();
    assert(!gauge.isReset());

    // Quick start (just should not crash)
    gauge.quickStart();

    printf("test_max17048: all assertions passed\n");
    return 0;
}
