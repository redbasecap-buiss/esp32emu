#pragma once
// esp32emu — painlessMesh mock (ESP-MESH networking)

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include <list>
#include <functional>

class painlessMesh {
public:
    using receivedCallback_t = std::function<void(uint32_t from, const std::string& msg)>;
    using newConnectionCallback_t = std::function<void(uint32_t nodeId)>;
    using droppedConnectionCallback_t = std::function<void(uint32_t nodeId)>;
    using changedConnectionsCallback_t = std::function<void()>;
    using nodeTimeAdjustedCallback_t = std::function<void(int32_t offset)>;

    void init(const std::string& prefix, const std::string& password, uint16_t port = 5555, int channel = 1, int authMode = 0) {
        _prefix = prefix;
        _port = port;
        (void)password; (void)channel; (void)authMode;
        printf("[esp32emu] painlessMesh: init('%s', port=%u)\n", prefix.c_str(), port);
    }

    void init(const std::string& prefix, const std::string& password, void* /*scheduler*/, uint16_t port = 5555, int channel = 1, int authMode = 0) {
        init(prefix, password, port, channel, authMode);
    }

    void update() { _updateCount++; }

    uint32_t getNodeId() const { return _nodeId; }

    std::list<uint32_t> getNodeList(bool includeThis = false) const {
        std::list<uint32_t> nodes(_connectedNodes.begin(), _connectedNodes.end());
        if (includeThis) nodes.push_front(_nodeId);
        return nodes;
    }

    size_t getNodeCount() const { return _connectedNodes.size(); }

    bool sendBroadcast(const std::string& msg, bool includeSelf = false) {
        printf("[esp32emu] painlessMesh: sendBroadcast('%s')\n", msg.c_str());
        _lastBroadcast = msg;
        _broadcastCount++;
        (void)includeSelf;
        return true;
    }

    bool sendSingle(uint32_t destId, const std::string& msg) {
        printf("[esp32emu] painlessMesh: sendSingle(%u, '%s')\n", destId, msg.c_str());
        _lastSingleDest = destId;
        _lastSingleMsg = msg;
        _singleCount++;
        return true;
    }

    uint32_t getNodeTime() const { return _nodeTime; }

    void setRoot(bool root = true) { _isRoot = root; }
    bool isRoot() const { return _isRoot; }

    void setContainsRoot(bool contains = true) { _containsRoot = contains; }

    void setDebugMsgTypes(uint16_t types) { _debugTypes = types; (void)types; }

    void onReceive(receivedCallback_t cb) { _onReceive = cb; }
    void onNewConnection(newConnectionCallback_t cb) { _onNewConn = cb; }
    void onDroppedConnection(droppedConnectionCallback_t cb) { _onDropConn = cb; }
    void onChangedConnections(changedConnectionsCallback_t cb) { _onChangedConn = cb; }
    void onNodeTimeAdjusted(nodeTimeAdjustedCallback_t cb) { _onTimeAdj = cb; }

    // === Test helpers ===
    void emu_setNodeId(uint32_t id) { _nodeId = id; }
    void emu_setNodeTime(uint32_t t) { _nodeTime = t; }

    void emu_simulateReceive(uint32_t from, const std::string& msg) {
        if (_onReceive) _onReceive(from, msg);
    }

    void emu_simulateNewConnection(uint32_t nodeId) {
        _connectedNodes.push_back(nodeId);
        if (_onNewConn) _onNewConn(nodeId);
        if (_onChangedConn) _onChangedConn();
    }

    void emu_simulateDropConnection(uint32_t nodeId) {
        _connectedNodes.erase(
            std::remove(_connectedNodes.begin(), _connectedNodes.end(), nodeId),
            _connectedNodes.end());
        if (_onDropConn) _onDropConn(nodeId);
        if (_onChangedConn) _onChangedConn();
    }

    void emu_simulateTimeAdjust(int32_t offset) {
        if (_onTimeAdj) _onTimeAdj(offset);
    }

    std::string emu_getLastBroadcast() const { return _lastBroadcast; }
    uint32_t emu_getBroadcastCount() const { return _broadcastCount; }
    std::string emu_getLastSingleMsg() const { return _lastSingleMsg; }
    uint32_t emu_getLastSingleDest() const { return _lastSingleDest; }
    uint32_t emu_getSingleCount() const { return _singleCount; }
    uint32_t emu_getUpdateCount() const { return _updateCount; }

private:
    std::string _prefix;
    uint16_t _port = 5555;
    uint32_t _nodeId = 12345678;
    uint32_t _nodeTime = 0;
    bool _isRoot = false;
    bool _containsRoot = false;
    uint16_t _debugTypes = 0;
    uint32_t _updateCount = 0;
    uint32_t _broadcastCount = 0;
    uint32_t _singleCount = 0;
    std::string _lastBroadcast;
    std::string _lastSingleMsg;
    uint32_t _lastSingleDest = 0;
    std::vector<uint32_t> _connectedNodes;

    receivedCallback_t _onReceive;
    newConnectionCallback_t _onNewConn;
    droppedConnectionCallback_t _onDropConn;
    changedConnectionsCallback_t _onChangedConn;
    nodeTimeAdjustedCallback_t _onTimeAdj;
};
