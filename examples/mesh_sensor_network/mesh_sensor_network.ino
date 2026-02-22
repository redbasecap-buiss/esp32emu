// painlessMesh — Sensor network with root node aggregation
#include <Arduino.h>
#include <painlessMesh.h>

painlessMesh mesh;
bool isRootNode = true;

void receivedCallback(uint32_t from, const std::string& msg) {
    Serial.printf("[ROOT] Sensor data from %u: %s\n", from, msg.c_str());
}

void droppedCallback(uint32_t nodeId) {
    Serial.printf("[ROOT] Lost sensor node: %u\n", nodeId);
}

void setup() {
    Serial.begin(115200);
    mesh.init("sensornet", "secure456", 5555);
    mesh.onReceive(receivedCallback);
    mesh.onDroppedConnection(droppedCallback);

    if (isRootNode) {
        mesh.setRoot(true);
        mesh.setContainsRoot(true);
        Serial.println("Root node started — aggregating sensor data");
    }

    // Simulate 3 sensor nodes
    mesh.emu_simulateNewConnection(200001);
    mesh.emu_simulateNewConnection(200002);
    mesh.emu_simulateNewConnection(200003);
    Serial.printf("Network size: %u nodes\n", (uint32_t)mesh.getNodeCount());
}

void loop() {
    mesh.update();

    // Simulate sensor readings arriving
    static int cycle = 0;
    if (cycle == 0) {
        mesh.emu_simulateReceive(200001, "{\"temp\":22.5,\"hum\":65}");
        mesh.emu_simulateReceive(200002, "{\"temp\":23.1,\"hum\":60}");
        mesh.emu_simulateReceive(200003, "{\"temp\":21.8,\"hum\":70}");
    }
    if (cycle == 2) {
        // Simulate node dropout
        mesh.emu_simulateDropConnection(200003);
    }

    auto nodes = mesh.getNodeList();
    Serial.printf("Active nodes: %u\n", (uint32_t)nodes.size());

    cycle++;
    delay(3000);
}
