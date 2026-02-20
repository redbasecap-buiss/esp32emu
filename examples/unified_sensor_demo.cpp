// Adafruit Unified Sensor — demo with a custom sensor implementation
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <cstring>

class MockHumiditySensor : public Adafruit_Sensor {
    float _humidity = 50.0f;
public:
    void setHumidity(float h) { _humidity = h; }

    bool getEvent(sensors_event_t *event) override {
        memset(event, 0, sizeof(sensors_event_t));
        event->version = sizeof(sensors_event_t);
        event->sensor_id = 1;
        event->type = SENSOR_TYPE_RELATIVE_HUMIDITY;
        event->timestamp = millis();
        event->relative_humidity = _humidity;
        return true;
    }

    void getSensor(sensor_t *sensor) override {
        memset(sensor, 0, sizeof(sensor_t));
        strncpy(sensor->name, "MockHum", sizeof(sensor->name) - 1);
        sensor->version = 1;
        sensor->sensor_id = 1;
        sensor->type = SENSOR_TYPE_RELATIVE_HUMIDITY;
        sensor->max_value = 100.0f;
        sensor->min_value = 0.0f;
        sensor->resolution = 0.1f;
    }
};

MockHumiditySensor humidity;

void setup() {
    Serial.begin(115200);
    humidity.printSensorDetails();
    Serial.println("Unified Sensor demo ready");
}

void loop() {
    sensors_event_t event;
    humidity.getEvent(&event);
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity, 1);
    Serial.println(" %");
    delay(2000);
}
