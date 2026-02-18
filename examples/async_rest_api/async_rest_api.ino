// Async REST API Example
// Demonstrates ESPAsyncWebServer with JSON endpoints
//
// Build: make build/examples/async_rest_api
// Run:   ./build/examples/async_rest_api --board esp32

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

int sensorValue = 0;
bool ledState = false;

void setup() {
    Serial.begin(115200);
    Serial.println("Async REST API Example");

    WiFi.begin("MyNetwork", "password");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

    // GET /api/status - returns JSON status
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* request) {
        String json = "{\"uptime\":";
        json += String(millis() / 1000);
        json += ",\"led\":";
        json += ledState ? "true" : "false";
        json += ",\"sensor\":";
        json += String(sensorValue);
        json += "}";
        request->send(200, "application/json", json.c_str());
    });

    // GET /api/sensor - read sensor
    server.on("/api/sensor", HTTP_GET, [](AsyncWebServerRequest* request) {
        String json = "{\"value\":" + String(sensorValue) + "}";
        request->send(200, "application/json", json.c_str());
    });

    // POST /api/led - toggle LED
    server.on("/api/led", HTTP_POST, [](AsyncWebServerRequest* request) {
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
        String json = "{\"led\":";
        json += ledState ? "true" : "false";
        json += "}";
        request->send(200, "application/json", json.c_str());
    });

    // Root page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(200, "text/html",
            "<h1>ESP32 Async API</h1>"
            "<p><a href='/api/status'>Status</a></p>"
            "<p><a href='/api/sensor'>Sensor</a></p>");
    });

    server.onNotFound([](AsyncWebServerRequest* request) {
        request->send(404, "text/plain", "Not Found");
    });

    server.begin();
    Serial.println("Async web server started!");
}

void loop() {
    sensorValue = analogRead(A0);
    delay(1000);
}
