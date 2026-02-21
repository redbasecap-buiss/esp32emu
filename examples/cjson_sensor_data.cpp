// esp32emu example — Build and parse sensor JSON with cJSON (ESP-IDF native)
#include <Arduino.h>
#include "cJSON.h"

void setup() {
    Serial.begin(115200);
    Serial.println("cJSON Sensor Data Example");

    // Build sensor report
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "device", "ESP32-Sensor-01");
    cJSON_AddNumberToObject(root, "uptime_ms", millis());

    cJSON* sensors = cJSON_CreateArray();

    cJSON* temp = cJSON_CreateObject();
    cJSON_AddStringToObject(temp, "type", "temperature");
    cJSON_AddNumberToObject(temp, "value", 23.5);
    cJSON_AddStringToObject(temp, "unit", "°C");
    cJSON_AddItemToArray(sensors, temp);

    cJSON* hum = cJSON_CreateObject();
    cJSON_AddStringToObject(hum, "type", "humidity");
    cJSON_AddNumberToObject(hum, "value", 62.1);
    cJSON_AddStringToObject(hum, "unit", "%");
    cJSON_AddItemToArray(sensors, hum);

    cJSON_AddItemToObject(root, "sensors", sensors);
    cJSON_AddBoolToObject(root, "battery_ok", 1);

    char* json = cJSON_Print(root);
    Serial.println("Generated JSON:");
    Serial.println(json);

    // Parse it back
    cJSON* parsed = cJSON_Parse(json);
    cJSON* device = cJSON_GetObjectItem(parsed, "device");
    Serial.print("Device: ");
    Serial.println(cJSON_GetStringValue(device));

    cJSON* s_arr = cJSON_GetObjectItem(parsed, "sensors");
    cJSON* elem = nullptr;
    cJSON_ArrayForEach(elem, s_arr) {
        const char* type = cJSON_GetStringValue(cJSON_GetObjectItem(elem, "type"));
        double val = cJSON_GetNumberValue(cJSON_GetObjectItem(elem, "value"));
        Serial.print("  ");
        Serial.print(type);
        Serial.print(": ");
        Serial.println(val);
    }

    free(json);
    cJSON_Delete(parsed);
    cJSON_Delete(root);
}

void loop() {
    delay(5000);
}
