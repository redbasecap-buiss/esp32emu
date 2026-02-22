#include "ElegantOTA.h"
#include <cassert>
#include <cstdio>

int main() {
    WebServer server(80);

    // Begin
    ElegantOTA.begin(&server, "admin", "secret");
    assert(ElegantOTA.isRunning());

    // Callbacks
    bool started = false, ended = false;
    bool endSuccess = false;
    size_t lastProgress = 0;

    ElegantOTA.onStart([&]() { started = true; });
    ElegantOTA.onEnd([&](bool success) { ended = true; endSuccess = success; });
    ElegantOTA.onProgress([&](size_t current, size_t total) {
        (void)total;
        lastProgress = current;
    });

    // Simulate successful update
    ElegantOTA._simulateUpdate(true);
    assert(started);
    assert(ended);
    assert(endSuccess);
    assert(lastProgress == 100);

    // Simulate failed update
    started = false; ended = false;
    ElegantOTA._simulateUpdate(false);
    assert(started);
    assert(ended);
    assert(!endSuccess);

    // Loop is no-op
    ElegantOTA.loop();

    // Auto reboot
    ElegantOTA.setAutoReboot(false);

    printf("test_elegantota: all assertions passed\n");
    return 0;
}
