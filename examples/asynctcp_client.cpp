// AsyncTCP client example — non-blocking HTTP request
#include <Arduino.h>
#include <AsyncTCP.h>

AsyncClient client;

void onConnect(void* arg, AsyncClient* c) {
    (void)arg;
    Serial.println("Connected!");
    c->write("GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n");
}

void onData(void* arg, AsyncClient* c, void* data, size_t len) {
    (void)arg; (void)c;
    Serial.printf("Received %zu bytes\n", len);
    Serial.write(static_cast<const char*>(data), len);
}

void onDisconnect(void* arg, AsyncClient* c) {
    (void)arg; (void)c;
    Serial.println("Disconnected");
}

void setup() {
    Serial.begin(115200);
    client.onConnect(onConnect);
    client.onData(onData);
    client.onDisconnect(onDisconnect);
    client.setNoDelay(true);
    client.connect("example.com", 80);
}

void loop() {
    delay(100);
}
