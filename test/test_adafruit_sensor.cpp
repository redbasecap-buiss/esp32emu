// esp32emu test — Adafruit Unified Sensor interface
#include "Adafruit_Sensor.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>

// Concrete test sensor implementation
class TestTempSensor : public Adafruit_Sensor {
public:
    float testTemp = 25.0f;

    bool getEvent(sensors_event_t *event) override {
        memset(event, 0, sizeof(sensors_event_t));
        event->version = 1;
        event->sensor_id = 42;
        event->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
        event->timestamp = 1000;
        event->temperature = testTemp;
        return true;
    }

    void getSensor(sensor_t *sensor) override {
        memset(sensor, 0, sizeof(sensor_t));
        strncpy(sensor->name, "TestTemp", sizeof(sensor->name) - 1);
        sensor->version = 1;
        sensor->sensor_id = 42;
        sensor->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
        sensor->max_value = 125.0f;
        sensor->min_value = -40.0f;
        sensor->resolution = 0.01f;
        sensor->min_delay = 2000;
    }
};

int main() {
    printf("=== Adafruit Unified Sensor Tests ===\n");

    TestTempSensor sensor;

    // Test getSensor
    sensor_t info;
    sensor.getSensor(&info);
    assert(strcmp(info.name, "TestTemp") == 0);
    assert(info.sensor_id == 42);
    assert(info.type == SENSOR_TYPE_AMBIENT_TEMPERATURE);
    assert(info.max_value == 125.0f);
    assert(info.min_value == -40.0f);
    printf("[PASS] getSensor returns correct metadata\n");

    // Test getEvent
    sensors_event_t event;
    assert(sensor.getEvent(&event));
    assert(event.version == 1);
    assert(event.sensor_id == 42);
    assert(event.type == SENSOR_TYPE_AMBIENT_TEMPERATURE);
    assert(fabs(event.temperature - 25.0f) < 0.01f);
    printf("[PASS] getEvent returns temperature reading\n");

    // Test updated value
    sensor.testTemp = 37.5f;
    assert(sensor.getEvent(&event));
    assert(fabs(event.temperature - 37.5f) < 0.01f);
    printf("[PASS] getEvent reflects updated value\n");

    // Test printSensorDetails (just verify no crash)
    sensor.printSensorDetails();
    printf("[PASS] printSensorDetails runs without crash\n");

    // Test sensor type enum values
    assert(SENSOR_TYPE_ACCELEROMETER == 1);
    assert(SENSOR_TYPE_PRESSURE == 6);
    assert(SENSOR_TYPE_RELATIVE_HUMIDITY == 12);
    assert(SENSOR_TYPE_AMBIENT_TEMPERATURE == 13);
    printf("[PASS] Sensor type enums have correct values\n");

    // Test event union fields
    sensors_event_t accelEvent;
    memset(&accelEvent, 0, sizeof(accelEvent));
    accelEvent.acceleration.x = 1.0f;
    accelEvent.acceleration.y = 2.0f;
    accelEvent.acceleration.z = 9.8f;
    assert(fabs(accelEvent.acceleration.z - 9.8f) < 0.01f);
    printf("[PASS] Event union acceleration fields work\n");

    printf("=== All Adafruit Unified Sensor tests passed! ===\n");
    return 0;
}
