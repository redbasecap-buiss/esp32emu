#pragma once
// esp32emu — PubSubClient (MQTT) mock
// Simulates MQTT publish/subscribe without a real broker.
// Messages published are delivered locally to matching subscriptions.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include "WiFiClient.h"

#define MQTT_VERSION_3_1_1 4
#define MQTT_MAX_PACKET_SIZE 256
#define MQTT_KEEPALIVE 15
#define MQTT_SOCKET_TIMEOUT 15

#define MQTT_CONNECTION_TIMEOUT     -4
#define MQTT_CONNECTION_LOST        -3
#define MQTT_CONNECT_FAILED         -2
#define MQTT_DISCONNECTED           -1
#define MQTT_CONNECTED               0
#define MQTT_CONNECT_BAD_PROTOCOL    1
#define MQTT_CONNECT_BAD_CLIENT_ID   2
#define MQTT_CONNECT_UNAVAILABLE     3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED    5

typedef std::function<void(char*, uint8_t*, unsigned int)> MQTT_CALLBACK_SIGNATURE;

class PubSubClient {
public:
    PubSubClient() = default;
    PubSubClient(WiFiClient& client) { (void)client; }

    PubSubClient& setServer(const char* host, uint16_t port) {
        server_ = host;
        port_ = port;
        return *this;
    }

    PubSubClient& setCallback(MQTT_CALLBACK_SIGNATURE cb) {
        callback_ = cb;
        return *this;
    }

    PubSubClient& setKeepAlive(uint16_t ka) { (void)ka; return *this; }
    PubSubClient& setSocketTimeout(uint16_t t) { (void)t; return *this; }
    PubSubClient& setBufferSize(uint16_t sz) { bufSize_ = sz; return *this; }
    uint16_t getBufferSize() const { return bufSize_; }

    bool connect(const char* id, const char* user = nullptr, const char* pass = nullptr) {
        (void)user; (void)pass;
        clientId_ = id ? id : "esp32emu";
        connected_ = true;
        fprintf(stderr, "[esp32emu] MQTT connected as '%s' to %s:%d\n",
                clientId_.c_str(), server_.c_str(), port_);
        return true;
    }

    bool connect(const char* id, const char* willTopic, uint8_t willQos,
                 bool willRetain, const char* willMessage,
                 const char* user = nullptr, const char* pass = nullptr) {
        (void)willTopic; (void)willQos; (void)willRetain; (void)willMessage;
        return connect(id, user, pass);
    }

    void disconnect() {
        connected_ = false;
        subscriptions_.clear();
        fprintf(stderr, "[esp32emu] MQTT disconnected\n");
    }

    bool connected() const { return connected_; }

    int state() const { return connected_ ? MQTT_CONNECTED : MQTT_DISCONNECTED; }

    bool publish(const char* topic, const char* payload, bool retained = false) {
        return publish(topic, (const uint8_t*)payload, strlen(payload), retained);
    }

    bool publish(const char* topic, const uint8_t* payload, unsigned int length, bool retained = false) {
        if (!connected_) return false;
        (void)retained;
        fprintf(stderr, "[esp32emu] MQTT publish: %s → %.*s\n", topic, (int)length, payload);

        std::lock_guard<std::mutex> lk(mtx_);
        published_.push_back({topic, std::string((const char*)payload, length)});

        // Deliver to local subscriptions
        if (callback_) {
            for (auto& sub : subscriptions_) {
                if (topicMatch(sub, topic)) {
                    std::string t(topic);
                    std::vector<uint8_t> p(payload, payload + length);
                    callback_(const_cast<char*>(t.c_str()), p.data(), length);
                }
            }
        }
        return true;
    }

    bool subscribe(const char* topic, uint8_t qos = 0) {
        if (!connected_) return false;
        (void)qos;
        std::lock_guard<std::mutex> lk(mtx_);
        subscriptions_.push_back(topic);
        fprintf(stderr, "[esp32emu] MQTT subscribe: %s\n", topic);
        return true;
    }

    bool unsubscribe(const char* topic) {
        if (!connected_) return false;
        std::lock_guard<std::mutex> lk(mtx_);
        subscriptions_.erase(
            std::remove(subscriptions_.begin(), subscriptions_.end(), std::string(topic)),
            subscriptions_.end());
        return true;
    }

    bool loop() {
        return connected_;
    }

    // ── Test helpers ──
    struct Message { std::string topic; std::string payload; };

    std::vector<Message> getPublished() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return published_;
    }

    std::vector<std::string> getSubscriptions() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return subscriptions_;
    }

    // Inject a message as if received from broker
    void injectMessage(const char* topic, const char* payload) {
        if (callback_) {
            callback_(const_cast<char*>(topic),
                      (uint8_t*)payload, strlen(payload));
        }
    }

private:
    bool topicMatch(const std::string& filter, const char* topic) const {
        // Simple wildcard matching: # at end matches all, + matches one level
        if (filter == "#") return true;
        if (filter == topic) return true;
        // Basic # suffix match
        if (filter.size() > 1 && filter.back() == '#' && filter[filter.size()-2] == '/') {
            return strncmp(topic, filter.c_str(), filter.size() - 1) == 0;
        }
        return false;
    }

    std::string server_ = "localhost";
    uint16_t port_ = 1883;
    std::string clientId_;
    bool connected_ = false;
    uint16_t bufSize_ = MQTT_MAX_PACKET_SIZE;
    MQTT_CALLBACK_SIGNATURE callback_;
    std::vector<std::string> subscriptions_;
    std::vector<Message> published_;
    mutable std::mutex mtx_;
};
