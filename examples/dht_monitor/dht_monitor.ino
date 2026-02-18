// DHT22 Temperature & Humidity Monitor
// Reads DHT22 sensor and prints readings every 2 seconds

#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    dht.begin();
    Serial.println("DHT22 Monitor Started");
}

void loop() {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    float hi = dht.computeHeatIndex(temp, hum, false);

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("°C  Humidity: ");
    Serial.print(hum);
    Serial.print("%  Heat Index: ");
    Serial.print(hi);
    Serial.println("°C");

    delay(2000);
}
