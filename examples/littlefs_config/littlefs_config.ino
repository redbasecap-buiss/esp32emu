// LittleFS config file example — store/load device settings
#include <Arduino.h>
#include <LittleFS.h>

struct Config {
    String deviceName = "ESP32-Device";
    int brightness = 128;
    bool ledEnabled = true;
};

Config config;

void saveConfig() {
    auto f = LittleFS.open("/config.txt", "w");
    if (!f) { Serial.println("Failed to save config"); return; }
    f.println(config.deviceName.c_str());
    f.println(String(config.brightness).c_str());
    f.println(config.ledEnabled ? "1" : "0");
    f.close();
    Serial.println("Config saved!");
}

void loadConfig() {
    if (!LittleFS.exists("/config.txt")) {
        Serial.println("No config file, using defaults");
        return;
    }
    auto f = LittleFS.open("/config.txt", "r");
    if (!f) return;
    auto content = f.readString();
    f.close();
    Serial.print("Loaded config: ");
    Serial.println(content.c_str());
}

void setup() {
    Serial.begin(115200);
    Serial.println("LittleFS Config Example");

    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed!");
        return;
    }

    loadConfig();
    config.deviceName = "MyESP32";
    config.brightness = 200;
    saveConfig();
    loadConfig();
}

void loop() {
    delay(5000);
}
