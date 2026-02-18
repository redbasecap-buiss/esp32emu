// esp32emu — PubSubClient (MQTT) test
#include "Arduino.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "PubSubClient.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

static std::vector<std::string> received;

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
    received.push_back(std::string(topic) + ":" + std::string((char*)payload, length));
}

int main() {
    WiFiClient wc;
    PubSubClient mqtt(wc);

    // Not connected yet
    assert(!mqtt.connected());
    assert(mqtt.state() == MQTT_DISCONNECTED);

    // Connect
    mqtt.setServer("test.mosquitto.org", 1883);
    mqtt.setCallback(mqttCallback);
    assert(mqtt.connect("test-client"));
    assert(mqtt.connected());
    assert(mqtt.state() == MQTT_CONNECTED);

    // Subscribe
    assert(mqtt.subscribe("sensor/temp"));
    assert(mqtt.subscribe("sensor/#"));
    auto subs = mqtt.getSubscriptions();
    assert(subs.size() == 2);

    // Publish (should trigger callback for matching subscriptions)
    received.clear();
    assert(mqtt.publish("sensor/temp", "23.5"));
    // Should match both "sensor/temp" and "sensor/#"
    assert(received.size() == 2);
    assert(received[0] == "sensor/temp:23.5");
    assert(received[1] == "sensor/temp:23.5");

    // Check published log
    auto pub = mqtt.getPublished();
    assert(pub.size() == 1);
    assert(pub[0].topic == "sensor/temp");
    assert(pub[0].payload == "23.5");

    // Inject message from "broker"
    received.clear();
    mqtt.injectMessage("sensor/humidity", "65");
    assert(received.size() == 1);
    assert(received[0] == "sensor/humidity:65");

    // Unsubscribe
    mqtt.unsubscribe("sensor/temp");
    subs = mqtt.getSubscriptions();
    assert(subs.size() == 1);

    // Loop
    assert(mqtt.loop());

    // Disconnect
    mqtt.disconnect();
    assert(!mqtt.connected());
    assert(!mqtt.publish("x", "y"));

    // Buffer size
    mqtt.setBufferSize(512);
    assert(mqtt.getBufferSize() == 512);

    printf("test_mqtt: all assertions passed\n");
    return 0;
}
