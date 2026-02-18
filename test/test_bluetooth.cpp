// esp32emu — BluetoothSerial test
#include "Arduino.h"
#include "BluetoothSerial.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    BluetoothSerial bt;

    // Not started
    assert(!bt.isReady());
    assert(bt.available() == 0);

    // Begin
    assert(bt.begin("TestESP32"));
    assert(bt.isReady());

    // No data yet
    assert(bt.available() == 0);
    assert(bt.read() == -1);

    // Inject data
    bt.injectRx("Hello BT");
    assert(bt.available() == 8);

    // Read byte by byte
    assert(bt.read() == 'H');
    assert(bt.available() == 7);

    // Read string
    std::string s = bt.readString();
    assert(s == "ello BT");
    assert(bt.available() == 0);

    // Write data
    bt.print("Response");
    assert(bt.getTxData() == "Response");
    bt.println(" OK");
    assert(bt.getTxData() == "Response OK\r\n");
    bt.clearTx();
    assert(bt.getTxData().empty());

    // readStringUntil
    bt.injectRx("line1\nline2\n");
    std::string l1 = bt.readStringUntil('\n');
    assert(l1 == "line1");
    std::string l2 = bt.readStringUntil('\n');
    assert(l2 == "line2");

    // readBytes
    bt.injectRx("ABCD");
    uint8_t buf[10];
    size_t n = bt.readBytes(buf, 3);
    assert(n == 3);
    assert(buf[0] == 'A' && buf[1] == 'B' && buf[2] == 'C');

    // Connection simulation
    assert(!bt.hasClient());
    bt.simulateConnect();
    assert(bt.hasClient());
    bt.disconnect();
    assert(!bt.hasClient());

    // Connect to remote
    assert(bt.connect("RemoteDevice"));
    assert(bt.hasClient());

    // End
    bt.end();
    assert(!bt.isReady());

    printf("test_bluetooth: all assertions passed\n");
    return 0;
}
