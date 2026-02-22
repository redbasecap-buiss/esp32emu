// test_lc709203f: LC709203F battery gauge mock tests
#include "Adafruit_LC709203F.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_LC709203F gauge;
    assert(gauge.begin());
    assert(gauge.initRSOC());

    // Defaults
    assert(fabs(gauge.cellVoltage() - 3.8f) < 0.01f);
    assert(fabs(gauge.cellPercent() - 80.0f) < 0.01f);

    // Version
    assert(gauge.getICversion() == 0x2717);

    // Test injection
    gauge.setTestVoltage(4.1f);
    assert(fabs(gauge.cellVoltage() - 4.1f) < 0.01f);
    gauge.setTestSOC(95.0f);
    assert(fabs(gauge.cellPercent() - 95.0f) < 0.01f);

    // Pack size
    gauge.setPackSize(LC709203F_APA_2000MAH);
    assert(gauge.getPackSize() == LC709203F_APA_2000MAH);
    gauge.setPackAPA(0x30);
    assert(gauge.getPackAPA() == 0x30);

    // Power mode
    assert(gauge.getPowerMode() == LC709203F_POWER_OPERATE);
    gauge.setPowerMode(LC709203F_POWER_SLEEP);
    assert(gauge.getPowerMode() == LC709203F_POWER_SLEEP);

    // Temperature mode
    gauge.setTemperatureMode(LC709203F_TEMP_THERMISTOR);
    assert(gauge.getTemperatureMode() == LC709203F_TEMP_THERMISTOR);

    // Alarm
    gauge.setAlarmRSOC(10);
    assert(gauge.getAlarmRSOC() == 10);
    gauge.setAlarmVoltage(3.3f);
    assert(fabs(gauge.getAlarmVoltage() - 3.3f) < 0.01f);

    // Cell profile
    gauge.setCellProfile(2);
    assert(gauge.getCellProfile() == 2);

    // Thermistor B
    gauge.setThermistorB(3950.0f);
    assert(fabs(gauge.getThermistorB() - 3950.0f) < 0.01f);

    printf("test_lc709203f: all assertions passed\n");
    return 0;
}
