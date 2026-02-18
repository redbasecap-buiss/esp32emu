#include "DHT.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // DHT22 basic
    DHT dht(4, DHT22);
    assert(dht.getPin() == 4);
    assert(dht.getType() == DHT22);

    // Before begin returns NAN
    assert(std::isnan(dht.readTemperature()));
    assert(std::isnan(dht.readHumidity()));

    dht.begin();

    // Default values
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    assert(t == 22.0f);
    assert(h == 50.0f);

    // Fahrenheit
    float tf = dht.readTemperature(true);
    assert(fabs(tf - 71.6f) < 0.1f);

    // Set values
    dht.setTemperature(30.5f);
    dht.setHumidity(80.0f);
    assert(dht.readTemperature() == 30.5f);
    assert(dht.readHumidity() == 80.0f);

    // Heat index
    float hi = dht.computeHeatIndex(30.5f, 80.0f, false);
    assert(hi > 30.0f); // Heat index should be higher than temperature at high humidity

    // DHT11
    DHT dht11(2, DHT11);
    dht11.begin();
    assert(dht11.getType() == DHT11);
    assert(dht11.readTemperature() == 22.0f);

    printf("test_dht: all assertions passed\n");
    return 0;
}
