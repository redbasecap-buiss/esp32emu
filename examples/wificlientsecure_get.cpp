// esp32emu example: WiFiClientSecure HTTPS GET
// Demonstrates secure TLS client connection.

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "MyNetwork";
const char* password = "MyPassword";
const char* host = "example.com";
const int port = 443;

WiFiClientSecure client;

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    // Skip certificate verification (for testing)
    client.setInsecure();

    Serial.printf("Connecting to %s:%d...\n", host, port);
    if (client.connect(host, port)) {
        Serial.println("Connected!");
        client.println("GET / HTTP/1.1");
        client.printf("Host: %s\r\n", host);
        client.println("Connection: close");
        client.println();

        while (client.connected()) {
            String line = client.readStringUntil('\n');
            Serial.println(line);
            if (line == "\r") break;
        }

        while (client.available()) {
            char c = client.read();
            Serial.write(c);
        }
    } else {
        Serial.println("Connection failed!");
    }

    client.stop();
}

void loop() {
    delay(10000);
}
