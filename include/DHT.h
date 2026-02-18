#pragma once
// esp32emu: DHT temperature/humidity sensor mock (DHT11, DHT22, AM2301)
#include <cstdint>
#include <cmath>

#define DHT11  11
#define DHT22  22
#define AM2301 21

class DHT {
public:
    DHT(uint8_t pin, uint8_t type, uint8_t /*count*/ = 6)
        : _pin(pin), _type(type),
          _temperature(22.0f), _humidity(50.0f), _begun(false) {}

    void begin() { _begun = true; }

    float readTemperature(bool fahrenheit = false) {
        if (!_begun) return NAN;
        if (fahrenheit) return _temperature * 1.8f + 32.0f;
        return _temperature;
    }

    float readHumidity() {
        if (!_begun) return NAN;
        return _humidity;
    }

    float computeHeatIndex(float temp, float humidity, bool fahrenheit = true) {
        if (!fahrenheit) temp = temp * 1.8f + 32.0f;
        // Simplified Rothfusz regression
        float hi = 0.5f * (temp + 61.0f + ((temp - 68.0f) * 1.2f) + (humidity * 0.094f));
        if (hi > 79.0f) {
            hi = -42.379f + 2.04901523f * temp + 10.14333127f * humidity
                 - 0.22475541f * temp * humidity - 0.00683783f * temp * temp
                 - 0.05481717f * humidity * humidity
                 + 0.00122874f * temp * temp * humidity
                 + 0.00085282f * temp * humidity * humidity
                 - 0.00000199f * temp * temp * humidity * humidity;
        }
        if (!fahrenheit) hi = (hi - 32.0f) / 1.8f;
        return hi;
    }

    // Test helpers
    void setTemperature(float t) { _temperature = t; }
    void setHumidity(float h) { _humidity = h; }
    uint8_t getPin() const { return _pin; }
    uint8_t getType() const { return _type; }

private:
    uint8_t _pin;
    uint8_t _type;
    float _temperature;
    float _humidity;
    bool _begun;
};
