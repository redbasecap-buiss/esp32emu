// esp32emu — AsyncTCP mock tests
#include "AsyncTCP.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test 1: client connect and write
    {
        AsyncClient client;
        bool connectFired = false;
        client.onConnect([](void* arg, AsyncClient*) {
            *static_cast<bool*>(arg) = true;
        }, &connectFired);

        assert(client.connect("example.com", 80));
        assert(client.connected());
        assert(connectFired);
        assert(client.space() > 0);
        assert(client.canSend());

        size_t w = client.write("GET / HTTP/1.1\r\n");
        assert(w == 16);
        assert(client.testGetTxBuffer() == "GET / HTTP/1.1\r\n");
    }

    // Test 2: client disconnect callback
    {
        AsyncClient client;
        bool disconnected = false;
        client.onDisconnect([](void* arg, AsyncClient*) {
            *static_cast<bool*>(arg) = true;
        }, &disconnected);

        client.connect("example.com", 80);
        client.close();
        assert(!client.connected());
        assert(disconnected);
    }

    // Test 3: data injection
    {
        AsyncClient client;
        std::string received;
        client.onData([](void* arg, AsyncClient*, void* data, size_t len) {
            static_cast<std::string*>(arg)->append(static_cast<char*>(data), len);
        }, &received);

        client.connect("example.com", 80);
        const char* resp = "HTTP/1.1 200 OK\r\n";
        client.testInjectData(resp, strlen(resp));
        assert(received == "HTTP/1.1 200 OK\r\n");
    }

    // Test 4: connection failure
    {
        AsyncClient client;
        client.testSetFailConnect(true);
        assert(!client.connect("fail.com", 80));
        assert(!client.connected());
        assert(client.write("data", 4) == 0);
    }

    // Test 5: server
    {
        AsyncServer server(8080);
        server.begin();
        assert(server.running());
        assert(server.port() == 8080);

        bool clientReceived = false;
        server.onClient([](void* arg, AsyncClient*) {
            *static_cast<bool*>(arg) = true;
        }, &clientReceived);

        AsyncClient fakeClient;
        server.testInjectClient(&fakeClient);
        assert(clientReceived);

        server.end();
        assert(!server.running());
    }

    // Test 6: client options
    {
        AsyncClient client;
        client.setNoDelay(true);
        assert(client.getNoDelay());
        client.setRxTimeout(5000);
        client.setAckTimeout(3000);
        client.connect("example.com", 443);
        assert(client.remotePort() == 443);
        assert(client.remoteIP() == 0x7F000001);
    }

    printf("test_asynctcp: all assertions passed\n");
    return 0;
}
