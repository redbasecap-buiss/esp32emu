// ArduinoJson REST API example — build and serve JSON responses
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

WebServer server(8080);

void handleStatus() {
    JsonDocument doc;
    doc["device"] = "esp32";
    doc["uptime"] = millis() / 1000;
    doc["heap"] = ESP.getFreeHeap();

    JsonObject sensors = doc.createNestedObject("sensors");
    sensors["temp"] = 23.5;
    sensors["humidity"] = 55;

    JsonArray pins = doc.createNestedArray("gpio");
    pins.add(digitalRead(2));
    pins.add(digitalRead(4));

    std::string json;
    serializeJson(doc, json);
    server.send(200, "application/json", json.c_str());
}

void handleConfig() {
    if (server.method() == HTTP_POST) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, server.arg("plain").c_str());
        if (err) {
            server.send(400, "text/plain", "Invalid JSON");
            return;
        }
        int interval = doc["interval"].as<int>();
        Serial.print("Config updated, interval=");
        Serial.println(interval);
        server.send(200, "text/plain", "OK");
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("MySSID", "password");
    while (WiFi.status() != WL_CONNECTED) delay(100);

    server.on("/status", HTTP_GET, handleStatus);
    server.on("/config", HTTP_POST, handleConfig);
    server.begin();
    Serial.println("JSON API server ready on :8080");
}

void loop() {
    server.handleClient();
    delay(10);
}
