// esp32emu example: ArduinoOTA wireless update
// Demonstrates OTA update configuration and lifecycle callbacks.

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

const char* ssid = "MyNetwork";
const char* password = "MyPassword";

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    ArduinoOTA.setHostname("esp32-sensor-node");
    ArduinoOTA.setPassword("admin");
    ArduinoOTA.setPort(3232);

    ArduinoOTA.onStart([]() {
        String type = "sketch";
        Serial.println("OTA Start: " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nOTA End — rebooting...");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    Serial.println("OTA ready");
}

void loop() {
    ArduinoOTA.handle();
    delay(10);
}
