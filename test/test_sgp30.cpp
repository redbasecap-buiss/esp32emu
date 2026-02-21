// esp32emu test — SGP30 Air Quality Sensor
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "SGP30.h"

int main() {
    // Test init
    {
        SGP30 sensor;
        assert(!sensor.test_isInitialized());
        assert(sensor.begin());
        assert(sensor.test_isInitialized());
    }

    // Test IAQ measurement
    {
        SGP30 sensor;
        assert(!sensor.IAQmeasure()); // not initialized
        sensor.begin();
        assert(sensor.IAQmeasure());
        assert(sensor.TVOC == 42);
        assert(sensor.eCO2 == 400);
    }

    // Test raw measurement
    {
        SGP30 sensor;
        sensor.begin();
        assert(sensor.IAQmeasureRaw());
        assert(sensor.rawH2 == 13000);
        assert(sensor.rawEthanol == 18000);
    }

    // Test serial number
    {
        SGP30 sensor;
        uint16_t serial[3];
        assert(sensor.getSerialNumber(serial));
        assert(serial[0] == 0x0000);
        assert(serial[1] == 0x0048);
        assert(serial[2] == 0xB6A0);
    }

    // Test baseline get/set
    {
        SGP30 sensor;
        sensor.begin();
        uint16_t eco2, tvoc;
        assert(sensor.getIAQBaseline(&eco2, &tvoc));
        assert(eco2 == 0x8A12);
        assert(tvoc == 0x9134);

        assert(sensor.setIAQBaseline(0x1234, 0x5678));
        assert(sensor.getIAQBaseline(&eco2, &tvoc));
        assert(eco2 == 0x1234);
        assert(tvoc == 0x5678);
    }

    // Test humidity compensation
    {
        SGP30 sensor;
        sensor.begin();
        sensor.setHumidity(13000);
        assert(sensor.IAQmeasure()); // should still work
    }

    // Test helpers
    {
        SGP30 sensor;
        sensor.begin();
        sensor.test_setTVOC(100);
        sensor.test_seteCO2(800);
        assert(sensor.TVOC == 100);
        assert(sensor.eCO2 == 800);

        sensor.test_setRaw(15000, 20000);
        assert(sensor.rawH2 == 15000);
        assert(sensor.rawEthanol == 20000);
    }

    printf("test_sgp30: all assertions passed\n");
    return 0;
}
