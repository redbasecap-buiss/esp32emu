#include "ESPmDNS.h"
#include <cassert>
#include <cstdio>

MDNSResponder MDNS;

int main() {
    assert(MDNS.begin("myesp"));
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "board", "esp32");
    MDNS.update();
    MDNS.end();

    printf("test_mdns: all assertions passed\n");
    return 0;
}
