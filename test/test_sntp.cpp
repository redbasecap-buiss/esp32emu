// esp32emu test — SNTP time sync
#include "esp_sntp.h"
#include <cassert>
#include <cstdio>
#include <ctime>
#include <cstring>

int main() {
    esp32emu::sntp_state().reset();

    // Not initialized yet
    assert(!sntp_enabled());
    assert(sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET);

    // Configure
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "time.google.com");
    assert(strcmp(sntp_getservername(0), "pool.ntp.org") == 0);
    assert(strcmp(sntp_getservername(1), "time.google.com") == 0);

    // Sync callback
    bool cb_called = false;
    esp32emu_sntp_set_sync_callback([&](struct timeval* tv) {
        cb_called = true;
        assert(tv != nullptr);
    });

    // Init triggers sync
    sntp_init();
    assert(sntp_enabled());
    assert(sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED);
    assert(cb_called);

    // Sync mode
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    assert(sntp_get_sync_mode() == SNTP_SYNC_MODE_SMOOTH);

    // Stop
    sntp_stop();
    assert(!sntp_enabled());
    assert(sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET);

    // esp_sntp aliases
    esp32emu::sntp_state().reset();
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "time.nist.gov");
    esp_sntp_init();
    assert(esp_sntp_enabled());
    assert(esp_sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED);
    esp_sntp_stop();

    // Host time works
    time_t now = time(nullptr);
    assert(now > 1000000000);  // after ~2001

    printf("test_sntp: all assertions passed\n");
    return 0;
}
