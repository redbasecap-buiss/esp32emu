#include "WiFiClientSecure.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test 1: WiFiClientSecure inherits from WiFiClient
    {
        WiFiClientSecure client;
        // Should be able to call all WiFiClient methods
        assert(!client.connected());
        assert(client.available() == 0);
    }

    // Test 2: Certificate methods (no-op but shouldn't crash)
    {
        WiFiClientSecure client;
        client.setInsecure();
        client.setCACert("-----BEGIN CERTIFICATE-----\ntest\n-----END CERTIFICATE-----");
        client.setCertificate("cert-data");
        client.setPrivateKey("key-data");
        // All no-ops in mock, just verify they don't crash
    }

    // Test 3: Connect and write (uses WiFiClient's real socket)
    {
        WiFiClientSecure client;
        // Connect to a port that likely isn't open — just test the API
        int result = client.connect("127.0.0.1", 19999);
        // May fail (no server), but API works
        (void)result;
        client.stop();
    }

    // Test 4: Polymorphism — can use as WiFiClient*
    {
        WiFiClientSecure secure;
        WiFiClient* base = &secure;
        assert(!base->connected());
    }

    printf("test_wificlientsecure: all assertions passed\n");
    return 0;
}
