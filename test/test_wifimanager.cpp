// esp32emu — WiFiManager mock tests
#include "WiFiManager.h"
#include <cassert>
#include <cstdio>

static bool apCallbackFired = false;
static bool saveCallbackFired = false;

static void resetFlags() {
    apCallbackFired = false;
    saveCallbackFired = false;
}

int main() {
    // Test 1: basic autoConnect succeeds
    {
        WiFiManager wm;
        assert(wm.autoConnect("TestAP", "password123"));
        assert(wm.testIsConnected());
    }

    // Test 2: autoConnect with forced portal triggers callbacks
    {
        resetFlags();
        WiFiManager wm;
        wm.setAPCallback([](WiFiManager*) { apCallbackFired = true; });
        wm.setSaveConfigCallback([]() { saveCallbackFired = true; });
        wm.testSetForcePortal(true);
        assert(wm.autoConnect("PortalAP"));
        assert(apCallbackFired);
        assert(saveCallbackFired);
        assert(wm.testIsConnected());
    }

    // Test 3: autoConnect failure
    {
        WiFiManager wm;
        wm.testSetFailConnect(true);
        bool failCbFired = false;
        wm.setAPClientFailCallback([&](WiFiManager*) { failCbFired = true; });
        assert(!wm.autoConnect("FailAP"));
        assert(!wm.testIsConnected());
        assert(failCbFired);
    }

    // Test 4: parameters
    {
        WiFiManager wm;
        WiFiManagerParameter p1("mqtt_server", "MQTT Server", "broker.local", 40);
        WiFiManagerParameter p2("mqtt_port", "MQTT Port", "1883", 6);
        wm.addParameter(&p1);
        wm.addParameter(&p2);

        assert(wm.getParametersCount() == 2);
        assert(std::string(p1.getValue()) == "broker.local");
        assert(std::string(p2.getValue()) == "1883");

        // Simulate user input
        p1.setValue("192.168.1.100");
        assert(std::string(p1.getValue()) == "192.168.1.100");
    }

    // Test 5: resetSettings
    {
        WiFiManager wm;
        wm.autoConnect("ResetAP");
        assert(wm.testIsConnected());
        wm.resetSettings();
        assert(!wm.testIsConnected());
    }

    // Test 6: startConfigPortal
    {
        resetFlags();
        WiFiManager wm;
        wm.setAPCallback([](WiFiManager*) { apCallbackFired = true; });
        wm.setSaveConfigCallback([]() { saveCallbackFired = true; });
        assert(wm.startConfigPortal("ConfigAP", "pass"));
        assert(apCallbackFired);
        assert(saveCallbackFired);
    }

    // Test 7: config options don't crash
    {
        WiFiManager wm;
        wm.setConfigPortalTimeout(120);
        wm.setConnectTimeout(30);
        wm.setTimeout(60);
        wm.setMinimumSignalQuality(50);
        wm.setRemoveDuplicateAPs(true);
        wm.setDebugOutput(false);
        wm.setConfigPortalBlocking(false);
        assert(wm.autoConnect());
    }

    // Test 8: getConfigPortalSSID
    {
        WiFiManager wm;
        wm.autoConnect("MyESP32");
        assert(String(wm.getConfigPortalSSID()) == "MyESP32");
    }

    printf("test_wifimanager: all assertions passed\n");
    return 0;
}
