// painlessMesh — Broadcast messages across mesh network
#include <Arduino.h>
#include <painlessMesh.h>

#define MESH_PREFIX   "esp32mesh"
#define MESH_PASSWORD "meshpass123"
#define MESH_PORT     5555

painlessMesh mesh;
uint32_t msgCount = 0;

void receivedCallback(uint32_t from, const std::string& msg) {
    Serial.printf("Received from %u: %s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("New connection: %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Connections changed. Nodes: %u\n", (uint32_t)mesh.getNodeCount());
}

void setup() {
    Serial.begin(115200);
    mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(receivedCallback);
    mesh.onNewConnection(newConnectionCallback);
    mesh.onChangedConnections(changedConnectionCallback);

    // Simulate some mesh nodes joining
    mesh.emu_simulateNewConnection(100001);
    mesh.emu_simulateNewConnection(100002);

    Serial.printf("My node ID: %u\n", mesh.getNodeId());
    Serial.println("Mesh Broadcast Demo Ready");
}

void loop() {
    mesh.update();

    // Broadcast a message every 5 seconds
    std::string msg = "Hello from " + std::to_string(mesh.getNodeId()) +
                      " msg#" + std::to_string(msgCount++);
    mesh.sendBroadcast(msg);

    // Simulate receiving a reply
    if (msgCount == 2) {
        mesh.emu_simulateReceive(100001, "Ack from node 100001");
    }

    delay(5000);
}
