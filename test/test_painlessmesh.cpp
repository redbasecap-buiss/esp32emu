// esp32emu test — painlessMesh mock
#include "Arduino.h"
#include "painlessMesh.h"
#include <cassert>
#include <cstdio>

int main() {
    painlessMesh mesh;
    mesh.init("esp32mesh", "password123", 5555);

    // Node ID
    mesh.emu_setNodeId(0xABCD1234);
    assert(mesh.getNodeId() == 0xABCD1234);

    // Initially no connections
    assert(mesh.getNodeCount() == 0);
    assert(mesh.getNodeList().empty());

    // Simulate connections
    uint32_t newConnId = 0;
    int changedCount = 0;
    mesh.onNewConnection([&](uint32_t id) { newConnId = id; });
    mesh.onChangedConnections([&]() { changedCount++; });

    mesh.emu_simulateNewConnection(1111);
    assert(newConnId == 1111);
    assert(changedCount == 1);
    assert(mesh.getNodeCount() == 1);

    mesh.emu_simulateNewConnection(2222);
    assert(mesh.getNodeCount() == 2);
    assert(changedCount == 2);

    // Node list with/without self
    auto nodes = mesh.getNodeList(false);
    assert(nodes.size() == 2);
    auto nodesWithSelf = mesh.getNodeList(true);
    assert(nodesWithSelf.size() == 3);

    // Drop connection
    uint32_t droppedId = 0;
    mesh.onDroppedConnection([&](uint32_t id) { droppedId = id; });
    mesh.emu_simulateDropConnection(1111);
    assert(droppedId == 1111);
    assert(mesh.getNodeCount() == 1);
    assert(changedCount == 3);

    // Receive message
    std::string receivedMsg;
    uint32_t receivedFrom = 0;
    mesh.onReceive([&](uint32_t from, const std::string& msg) {
        receivedFrom = from;
        receivedMsg = msg;
    });
    mesh.emu_simulateReceive(2222, "hello mesh");
    assert(receivedFrom == 2222);
    assert(receivedMsg == "hello mesh");

    // Send broadcast
    mesh.sendBroadcast("broadcast msg");
    assert(mesh.emu_getLastBroadcast() == "broadcast msg");
    assert(mesh.emu_getBroadcastCount() == 1);

    // Send single
    mesh.sendSingle(2222, "direct msg");
    assert(mesh.emu_getLastSingleDest() == 2222);
    assert(mesh.emu_getLastSingleMsg() == "direct msg");
    assert(mesh.emu_getSingleCount() == 1);

    // Root
    mesh.setRoot(true);
    assert(mesh.isRoot());

    // Update counter
    assert(mesh.emu_getUpdateCount() == 0);
    mesh.update();
    mesh.update();
    assert(mesh.emu_getUpdateCount() == 2);

    // Time adjustment callback
    int32_t adjOffset = 0;
    mesh.onNodeTimeAdjusted([&](int32_t offset) { adjOffset = offset; });
    mesh.emu_simulateTimeAdjust(-500);
    assert(adjOffset == -500);

    // Node time
    mesh.emu_setNodeTime(123456);
    assert(mesh.getNodeTime() == 123456);

    printf("test_painlessmesh: all assertions passed\n");
    return 0;
}
