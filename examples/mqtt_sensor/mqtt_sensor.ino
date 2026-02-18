// MQTT Sensor — Publishes temperature readings and subscribes to control commands
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqtt(espClient);

float temperature = 22.0;
bool ledState = false;

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
    String msg;
    for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

    Serial.print("MQTT received [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(msg.c_str());

    if (String(topic) == "home/esp32/led") {
        ledState = (msg == "ON");
        digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
        Serial.print("LED: ");
        Serial.println(ledState ? "ON" : "OFF");
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    WiFi.begin("MyNetwork", "password");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    mqtt.setServer("broker.local", 1883);
    mqtt.setCallback(mqttCallback);

    if (mqtt.connect("esp32-sensor")) {
        Serial.println("MQTT connected!");
        mqtt.subscribe("home/esp32/led");
        mqtt.subscribe("home/esp32/config/#");
    }
}

void loop() {
    mqtt.loop();

    // Simulate temperature drift
    temperature += (random(-10, 11) / 10.0);
    temperature = constrain(temperature, 15.0, 35.0);

    char payload[32];
    snprintf(payload, sizeof(payload), "%.1f", temperature);
    mqtt.publish("home/esp32/temperature", payload);

    Serial.print("Temperature: ");
    Serial.print(payload);
    Serial.println(" °C");

    delay(2000);
}
