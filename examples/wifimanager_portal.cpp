// WiFiManager captive portal provisioning example
#include <Arduino.h>
#include <WiFiManager.h>

WiFiManager wm;
WiFiManagerParameter mqttServer("mqtt", "MQTT Server", "broker.local", 40);
WiFiManagerParameter mqttPort("port", "MQTT Port", "1883", 6);

bool shouldSaveConfig = false;

void saveConfigCallback() {
    Serial.println("Config saved!");
    shouldSaveConfig = true;
}

void setup() {
    Serial.begin(115200);
    Serial.println("WiFiManager Provisioning Example");

    wm.addParameter(&mqttServer);
    wm.addParameter(&mqttPort);
    wm.setSaveConfigCallback(saveConfigCallback);
    wm.setConfigPortalTimeout(180);

    if (!wm.autoConnect("ESP32_Setup", "configure")) {
        Serial.println("Failed to connect — restarting");
        ESP.restart();
    }

    Serial.println("Connected to WiFi!");
    Serial.print("MQTT Server: ");
    Serial.println(mqttServer.getValue());
    Serial.print("MQTT Port: ");
    Serial.println(mqttPort.getValue());
}

void loop() {
    delay(1000);
}
