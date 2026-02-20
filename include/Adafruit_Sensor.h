#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>

// Adafruit Unified Sensor — abstract base class for sensor drivers
// Many Adafruit libraries depend on this interface

typedef enum {
    SENSOR_TYPE_ACCELEROMETER     = 1,
    SENSOR_TYPE_MAGNETIC_FIELD    = 2,
    SENSOR_TYPE_ORIENTATION       = 3,
    SENSOR_TYPE_GYROSCOPE         = 4,
    SENSOR_TYPE_LIGHT             = 5,
    SENSOR_TYPE_PRESSURE          = 6,
    SENSOR_TYPE_PROXIMITY         = 8,
    SENSOR_TYPE_GRAVITY           = 9,
    SENSOR_TYPE_LINEAR_ACCELERATION = 10,
    SENSOR_TYPE_ROTATION_VECTOR   = 11,
    SENSOR_TYPE_RELATIVE_HUMIDITY = 12,
    SENSOR_TYPE_AMBIENT_TEMPERATURE = 13,
    SENSOR_TYPE_VOLTAGE           = 15,
    SENSOR_TYPE_CURRENT           = 16,
    SENSOR_TYPE_COLOR             = 17,
    SENSOR_TYPE_TVOC              = 18,
    SENSOR_TYPE_VOC_INDEX         = 19,
    SENSOR_TYPE_NOX_INDEX         = 20,
    SENSOR_TYPE_CO2               = 21,
    SENSOR_TYPE_ECO2              = 22,
    SENSOR_TYPE_PM10_STD          = 23,
    SENSOR_TYPE_PM25_STD          = 24,
    SENSOR_TYPE_PM100_STD         = 25,
    SENSOR_TYPE_PM10_ENV          = 26,
    SENSOR_TYPE_PM25_ENV          = 27,
    SENSOR_TYPE_PM100_ENV         = 28,
    SENSOR_TYPE_GAS_RESISTANCE    = 29,
    SENSOR_TYPE_UNITLESS          = 30,
    SENSOR_TYPE_ALTITUDE          = 31,
} sensors_type_t;

typedef struct {
    char     name[12];
    int32_t  version;
    int32_t  sensor_id;
    int32_t  type;
    float    max_value;
    float    min_value;
    float    resolution;
    int32_t  min_delay;
} sensor_t;

typedef struct {
    int32_t  version;
    int32_t  sensor_id;
    int32_t  type;
    int32_t  reserved0;
    uint32_t timestamp;
    union {
        float data[4];
        struct {
            float x;
            float y;
            float z;
        } acceleration;
        struct {
            float x;
            float y;
            float z;
        } magnetic;
        struct {
            float roll;
            float pitch;
            float heading;
        } orientation;
        struct {
            float x;
            float y;
            float z;
        } gyro;
        float temperature;
        float distance;
        float light;
        float pressure;
        float relative_humidity;
        float current;
        float voltage;
        float tvoc;
        float voc_index;
        float nox_index;
        float CO2;
        float eCO2;
        float pm10_std;
        float pm25_std;
        float pm100_std;
        float pm10_env;
        float pm25_env;
        float pm100_env;
        float gas_resistance;
        float unitless;
        float altitude;
        struct {
            float r;
            float g;
            float b;
            float rgba;
        } color;
    };
} sensors_event_t;

class Adafruit_Sensor {
public:
    virtual ~Adafruit_Sensor() {}
    virtual bool getEvent(sensors_event_t *event) = 0;
    virtual void getSensor(sensor_t *sensor) = 0;

    // Test helper: print sensor info
    void printSensorDetails() {
        sensor_t s;
        getSensor(&s);
        printf("Sensor:       %s\n", s.name);
        printf("Type:         %d\n", s.type);
        printf("Driver Ver:   %d\n", s.version);
        printf("Unique ID:    %d\n", s.sensor_id);
        printf("Max Value:    %.2f\n", s.max_value);
        printf("Min Value:    %.2f\n", s.min_value);
        printf("Resolution:   %.4f\n", s.resolution);
    }
};
