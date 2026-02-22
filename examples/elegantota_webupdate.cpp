// ElegantOTA — web-based OTA firmware update
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ElegantOTA.h>

WebServer server(80);

void setup() {
    Serial.begin(115200);
    WiFi.begin("MyNetwork", "password");

    server.on("/", []() {
        server.send(200, "text/html",
            "<h1>ESP32 OTA Server</h1>"
            "<p>Visit <a href='/update'>/update</a> for firmware upload</p>");
    });

    ElegantOTA.begin(&server, "admin", "secret");
    ElegantOTA.onStart([]() { Serial.println("OTA update starting..."); });
    ElegantOTA.onProgress([](size_t current, size_t total) {
        Serial.printf("Progress: %zu/%zu bytes\n", current, total);
    });
    ElegantOTA.onEnd([](bool success) {
        Serial.printf("OTA update %s\n", success ? "succeeded!" : "FAILED");
    });

    server.begin();
    Serial.println("Server started. OTA at /update");
}

void loop() {
    server.handleClient();
    ElegantOTA.loop();
}
