// esp32emu — MCP2515 CAN bus test
#include "MCP2515.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    MCP2515 can(5);
    assert(!can._isInitialized());

    // Init
    assert(can.reset() == MCP2515_NS::ERROR_OK);
    assert(can._isInitialized());
    assert(can.setBitrate(MCP2515_NS::CAN_500KBPS) == MCP2515_NS::ERROR_OK);
    assert(can.setNormalMode() == MCP2515_NS::ERROR_OK);

    // Send
    can_frame tx = {};
    tx.can_id = 0x123;
    tx.can_dlc = 4;
    tx.data[0] = 0xDE; tx.data[1] = 0xAD; tx.data[2] = 0xBE; tx.data[3] = 0xEF;
    assert(can.sendMessage(&tx) == MCP2515_NS::ERROR_OK);
    assert(can._getTxCount() == 1);

    // No message to read in normal mode
    can_frame rx = {};
    assert(can.readMessage(&rx) == MCP2515_NS::ERROR_NOMSG);
    assert(!can.checkReceive());

    // Loopback mode
    assert(can.setLoopbackMode() == MCP2515_NS::ERROR_OK);
    assert(can._getMode() == 1);
    assert(can.sendMessage(&tx) == MCP2515_NS::ERROR_OK);
    assert(can.checkReceive());
    assert(can.readMessage(&rx) == MCP2515_NS::ERROR_OK);
    assert(rx.can_id == 0x123);
    assert(rx.can_dlc == 4);
    assert(rx.data[0] == 0xDE);

    // Inject frame
    can_frame injected = {};
    injected.can_id = 0x456;
    injected.can_dlc = 2;
    injected.data[0] = 0xAA;
    injected.data[1] = 0xBB;
    can._injectFrame(injected);
    assert(can.checkReceive());
    assert(can.getStatus() & MCP2515_NS::RX0IF);
    can_frame out = {};
    assert(can.readMessage(&out) == MCP2515_NS::ERROR_OK);
    assert(out.can_id == 0x456);

    // Filters
    assert(can.setFilterMask(MCP2515_NS::MASK0, false, 0x7FF) == MCP2515_NS::ERROR_OK);
    assert(can.setFilter(MCP2515_NS::RXF0, false, 0x123) == MCP2515_NS::ERROR_OK);

    // Error counts
    can._setErrorCountRX(5);
    can._setErrorCountTX(3);
    assert(can.errorCountRX() == 5);
    assert(can.errorCountTX() == 3);

    // Listen only mode
    assert(can.setListenOnlyMode() == MCP2515_NS::ERROR_OK);
    assert(can._getMode() == 2);

    printf("test_mcp2515: all assertions passed\n");
    return 0;
}
