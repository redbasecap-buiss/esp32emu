#include "DallasTemperature.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.1f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Device discovery via OneWire
    {
        OneWire ow(4);
        uint8_t addr1[8] = {0x28, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        uint8_t addr2[8] = {0x28, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
        ow.addDevice(addr1);
        ow.addDevice(addr2);

        DallasTemperature sensors(&ow);
        sensors.begin();
        assert(sensors.getDeviceCount() == 2);
    }

    // Test 2: Temperature read by index
    {
        OneWire ow(4);
        uint8_t addr[8] = {0x28, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};
        ow.addDevice(addr);

        DallasTemperature sensors(&ow);
        sensors.begin();
        sensors.setTemperatureByIndex(0, 25.5f);
        sensors.requestTemperatures();

        assert(near(sensors.getTempCByIndex(0), 25.5f));
        assert(near(sensors.getTempFByIndex(0), 77.9f));
    }

    // Test 3: Temperature read by address
    {
        OneWire ow(4);
        uint8_t addr[8] = {0x28, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        ow.addDevice(addr);

        DallasTemperature sensors(&ow);
        sensors.begin();
        sensors.setTemperature(addr, -10.0f);

        assert(near(sensors.getTempC(addr), -10.0f));
        assert(near(sensors.getTempF(addr), 14.0f));
    }

    // Test 4: Default temperature
    {
        OneWire ow(4);
        uint8_t addr[8] = {0x28, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        ow.addDevice(addr);

        DallasTemperature sensors(&ow);
        sensors.begin();
        // Default is 22.5°C
        assert(near(sensors.getTempCByIndex(0), 22.5f));
    }

    // Test 5: Resolution and conversion settings
    {
        DallasTemperature sensors;
        sensors.setResolution(9);
        assert(sensors.getResolution() == 9);
        sensors.setWaitForConversion(false);
        assert(!sensors.getWaitForConversion());
    }

    // Test 6: Invalid index returns -127
    {
        DallasTemperature sensors;
        sensors.begin();
        assert(near(sensors.getTempCByIndex(5), -127.0f));
    }

    // Test 7: getAddress
    {
        OneWire ow(4);
        uint8_t addr[8] = {0x28, 0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03};
        ow.addDevice(addr);

        DallasTemperature sensors(&ow);
        sensors.begin();

        uint8_t found[8];
        assert(sensors.getAddress(found, 0));
        assert(found[0] == 0x28);
        assert(found[1] == 0xDE);
        assert(!sensors.getAddress(found, 1)); // out of range
    }

    printf("test_dallas: all assertions passed\n");
    return 0;
}
