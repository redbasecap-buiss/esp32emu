#include "WiFiMulti.h"
#include <cassert>

int main() {
    WiFiMulti wifiMulti;

    // Add multiple APs
    assert(wifiMulti.addAP("Network1", "pass1"));
    assert(wifiMulti.addAP("Network2", "pass2"));
    assert(wifiMulti.addAP("OpenNetwork"));
    assert(wifiMulti.APcount() == 3);

    // Not connected yet
    assert(wifiMulti.connectedIndex() == -1);

    // Run connects to first AP
    uint8_t status = wifiMulti.run();
    assert(status == WL_CONNECTED);
    assert(wifiMulti.connectedIndex() == 0);

    printf("test_wifimulti: all assertions passed\n");
    return 0;
}
