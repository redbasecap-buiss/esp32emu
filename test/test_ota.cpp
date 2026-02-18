// esp32emu — ArduinoOTA test
#include "Arduino.h"
#include "ArduinoOTA.h"
#include <cassert>
#include <cstdio>
#include <vector>

static bool startCalled = false;
static bool endCalled = false;
static std::vector<unsigned int> progressValues;
static ota_error_t lastError;
static bool errorCalled = false;

int main() {
    // Setup callbacks
    ArduinoOTA.onStart([]() { startCalled = true; });
    ArduinoOTA.onEnd([]() { endCalled = true; });
    ArduinoOTA.onProgress([](unsigned int cur, unsigned int total) {
        progressValues.push_back(cur);
        (void)total;
    });
    ArduinoOTA.onError([](ota_error_t err) {
        errorCalled = true;
        lastError = err;
    });

    // Configure
    ArduinoOTA.setHostname("test-esp32");
    ArduinoOTA.setPort(3232);
    ArduinoOTA.setPassword("secret");

    assert(ArduinoOTA.getHostname() == "test-esp32");

    // Begin
    ArduinoOTA.begin();
    assert(ArduinoOTA.isStarted());

    // Handle does nothing (no crash)
    ArduinoOTA.handle();

    // Simulate successful OTA
    ArduinoOTA.simulateOTA(true);
    assert(startCalled);
    assert(endCalled);
    assert(progressValues.size() == 5); // 0, 25k, 50k, 75k, 100k
    assert(progressValues[0] == 0);
    assert(progressValues.back() == 100000);
    assert(!errorCalled);

    // Simulate failed OTA
    startCalled = false;
    endCalled = false;
    progressValues.clear();
    ArduinoOTA.simulateOTA(false);
    assert(startCalled);
    assert(!endCalled);
    assert(errorCalled);
    assert(lastError == OTA_RECEIVE_ERROR);

    printf("test_ota: all assertions passed\n");
    return 0;
}
