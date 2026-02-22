// WiFiMulti — connect to the strongest available network
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

void setup() {
    Serial.begin(115200);

    wifiMulti.addAP("HomeNetwork", "password1");
    wifiMulti.addAP("OfficeNetwork", "password2");
    wifiMulti.addAP("MobileHotspot", "password3");

    Serial.println("Connecting to best available network...");

    if (wifiMulti.run() == WL_CONNECTED) {
        Serial.print("Connected to: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP: ");
        Serial.println(WiFi.localIP().toString().c_str());
    } else {
        Serial.println("Connection failed!");
    }
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnecting...");
        wifiMulti.run();
    }
    delay(5000);
}
