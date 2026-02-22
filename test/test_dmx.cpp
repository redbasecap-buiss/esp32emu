// esp32emu test — ESP-DMX mock
#include "Arduino.h"
#include "esp_dmx.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    dmx::DMX dmxPort;

    // Not installed yet
    assert(!dmxPort.isInstalled());
    assert(dmxPort.send() == DMX_ERR_NOT_INIT);
    assert(dmxPort.receive() == DMX_ERR_NOT_INIT);

    // Install
    dmx::Config cfg;
    cfg.uart_num = 1;
    cfg.tx_pin = 17;
    cfg.rx_pin = 16;
    cfg.en_pin = 21;
    cfg.type = DMX_TYPE_SEND;
    assert(dmxPort.install(cfg) == DMX_OK);
    assert(dmxPort.isInstalled());

    // Write channels
    dmxPort.write(1, 255);  // channel 1 full
    dmxPort.write(2, 128);
    dmxPort.write(512, 42);
    assert(dmxPort.read(1) == 255);
    assert(dmxPort.read(2) == 128);
    assert(dmxPort.read(512) == 42);
    assert(dmxPort.read(0) == 0);  // start code

    // Out of range
    dmxPort.write(0, 99);  // channel 0 is start code area, ignored by write
    dmxPort.write(513, 99);  // out of range, ignored

    // Start code
    dmxPort.setStartCode(0x00);
    assert(dmxPort.getStartCode() == 0x00);

    // Send
    assert(dmxPort.send() == DMX_OK);
    assert(dmxPort.emu_getPacketsSent() == 1);
    assert(dmxPort.send() == DMX_OK);
    assert(dmxPort.emu_getPacketsSent() == 2);

    // Receive with no data
    assert(dmxPort.receive() == DMX_ERR_TIMEOUT);

    // Inject incoming packet
    uint8_t incoming[DMX_MAX_PACKET_SIZE] = {};
    incoming[0] = 0x00;  // start code
    incoming[1] = 200;
    incoming[2] = 100;
    incoming[10] = 50;
    dmxPort.emu_injectPacket(incoming, DMX_MAX_PACKET_SIZE);
    assert(dmxPort.receive() == DMX_OK);
    assert(dmxPort.read(1) == 200);
    assert(dmxPort.read(2) == 100);
    assert(dmxPort.read(10) == 50);

    // Second receive without new injection times out
    assert(dmxPort.receive() == DMX_ERR_TIMEOUT);

    // Read all
    uint8_t buf[DMX_MAX_PACKET_SIZE] = {};
    dmxPort.readAll(buf, DMX_MAX_PACKET_SIZE);
    assert(buf[1] == 200);
    assert(buf[10] == 50);

    // Write all
    uint8_t newData[DMX_MAX_PACKET_SIZE] = {};
    newData[1] = 77;
    newData[100] = 88;
    dmxPort.writeAll(newData, DMX_MAX_PACKET_SIZE);
    assert(dmxPort.read(1) == 77);
    assert(dmxPort.read(100) == 88);

    // Uninstall
    assert(dmxPort.uninstall() == DMX_OK);
    assert(!dmxPort.isInstalled());

    // Test helper
    dmxPort.install(cfg);
    dmxPort.emu_setChannelData(5, 123);
    assert(dmxPort.emu_getChannel(5) == 123);

    printf("test_dmx: all assertions passed\n");
    return 0;
}
