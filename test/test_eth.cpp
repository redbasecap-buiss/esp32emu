#include "ETH.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test basic begin
    {
        ETH.reset();
        assert(ETH.begin());
        assert(ETH.linkUp());
        assert(ETH.connected());
    }

    // Test IP addresses
    {
        ETH.reset();
        ETH.begin();
        IPAddress ip = ETH.localIP();
        assert(ip[0] == 192 && ip[1] == 168 && ip[2] == 1 && ip[3] == 100);
        IPAddress gw = ETH.gatewayIP();
        assert(gw[0] == 192 && gw[3] == 1);
        IPAddress dns = ETH.dnsIP();
        assert(dns[0] == 8 && dns[1] == 8);
    }

    // Test link speed and duplex
    {
        ETH.reset();
        ETH.begin();
        assert(ETH.linkSpeed() == 100);
        assert(ETH.fullDuplex());
        ETH.setLinkSpeed(10);
        assert(ETH.linkSpeed() == 10);
        ETH.setFullDuplex(false);
        assert(!ETH.fullDuplex());
    }

    // Test hostname
    {
        ETH.reset();
        ETH.begin();
        assert(ETH.setHostname("my-esp32"));
        assert(strcmp(ETH.getHostname(), "my-esp32") == 0);
    }

    // Test MAC address
    {
        ETH.reset();
        ETH.begin();
        String mac = ETH.macAddress();
        assert(mac.length() > 0);
    }

    // Test link down
    {
        ETH.reset();
        ETH.begin();
        ETH.setLinkUp(false);
        assert(!ETH.linkUp());
        assert(!ETH.connected());
    }

    // Test subnet mask
    {
        ETH.reset();
        ETH.begin();
        IPAddress mask = ETH.subnetMask();
        assert(mask[0] == 255 && mask[1] == 255 && mask[2] == 255 && mask[3] == 0);
    }

    // Test PHY types
    {
        ETH.reset();
        assert(ETH.begin(0, -1, 23, 18, ETH_PHY_W5500));
        assert(ETH.connected());
    }

    printf("test_eth: all assertions passed\n");
    return 0;
}
