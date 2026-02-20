#include "ArduinoOTA.h"
#include <cassert>
#include <cstdio>
#include <string>

int main() {
    // Test 1: Configuration
    {
        // Reset for fresh state
        auto& ota = ArduinoOTAClass::instance();
        ota.setHostname("test-device");
        ota.setPassword("secret");
        ota.setPort(8266);
        assert(ota.getHostname() == "test-device");
    }

    // Test 2: Lifecycle callbacks on successful OTA
    {
        auto& ota = ArduinoOTAClass::instance();
        ota.setHostname("ota-test");

        bool startCalled = false;
        bool endCalled = false;
        int progressCalls = 0;
        bool errorCalled = false;

        ota.onStart([&]() { startCalled = true; });
        ota.onEnd([&]() { endCalled = true; });
        ota.onProgress([&](unsigned int current, unsigned int total) {
            (void)current; (void)total;
            progressCalls++;
        });
        ota.onError([&](ota_error_t err) {
            (void)err;
            errorCalled = true;
        });

        ota.begin();
        assert(ota.isStarted());

        // handle() is a no-op
        ota.handle();

        // Simulate successful OTA
        ota.simulateOTA(true);
        assert(startCalled);
        assert(endCalled);
        assert(progressCalls > 0);
        assert(!errorCalled);
    }

    // Test 3: Failed OTA triggers error callback
    {
        auto& ota = ArduinoOTAClass::instance();

        bool errorCalled = false;
        ota_error_t lastError = OTA_AUTH_ERROR;

        ota.onError([&](ota_error_t err) {
            errorCalled = true;
            lastError = err;
        });

        ota.simulateOTA(false);
        assert(errorCalled);
        assert(lastError == OTA_RECEIVE_ERROR);
    }

    printf("test_arduinoota: all assertions passed\n");
    return 0;
}
