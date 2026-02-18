// test_arduinojson: ArduinoJson mock tests
#include "ArduinoJson.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test basic serialization
    {
        JsonDocument doc;
        doc["name"] = "esp32";
        doc["version"] = 3;
        doc["active"] = true;
        doc["temp"] = 23.5;
        std::string out;
        serializeJson(doc, out);
        assert(out.find("\"name\":\"esp32\"") != std::string::npos);
        assert(out.find("\"version\":3") != std::string::npos);
        assert(out.find("\"active\":true") != std::string::npos);
    }

    // Test deserialization
    {
        JsonDocument doc;
        const char* json = R"({"sensor":"dht22","temp":25.4,"humidity":60,"ok":true})";
        DeserializationError err = deserializeJson(doc, json);
        assert(!err);
        assert(strcmp(doc["sensor"].as<const char*>(), "dht22") == 0);
        assert(doc["temp"].as<float>() > 25.0f && doc["temp"].as<float>() < 26.0f);
        assert(doc["humidity"].as<int>() == 60);
        assert(doc["ok"].as<bool>() == true);
    }

    // Test nested objects
    {
        JsonDocument doc;
        doc["device"] = "esp32";
        JsonObject sensor = doc.createNestedObject("sensor");
        sensor["type"] = "bmp280";
        sensor["temp"] = 22.1;
        std::string out;
        serializeJson(doc, out);
        assert(out.find("\"sensor\":{") != std::string::npos);
        assert(out.find("\"type\":\"bmp280\"") != std::string::npos);
    }

    // Test arrays
    {
        JsonDocument doc;
        JsonArray arr = doc.createNestedArray("values");
        arr.add(10);
        arr.add(20);
        arr.add(30);
        std::string out;
        serializeJson(doc, out);
        assert(out.find("[10,20,30]") != std::string::npos);
    }

    // Test deserialize array
    {
        JsonDocument doc;
        deserializeJson(doc, "[1,2,3]");
        assert(doc.size() == 3);
        assert(doc[0].as<int>() == 1);
        assert(doc[2].as<int>() == 3);
    }

    // Test error handling
    {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, "");
        assert(err);
        err = deserializeJson(doc, "{invalid}");
        assert(err);
    }

    // Test containsKey
    {
        JsonDocument doc;
        deserializeJson(doc, R"({"a":1,"b":2})");
        assert(doc.containsKey("a"));
        assert(!doc.containsKey("c"));
    }

    // Test serialize to char buffer
    {
        JsonDocument doc;
        doc["x"] = 42;
        char buf[64];
        serializeJson(doc, buf, sizeof(buf));
        assert(strstr(buf, "\"x\":42") != nullptr);
    }

    printf("test_arduinojson: all assertions passed\n");
    return 0;
}
