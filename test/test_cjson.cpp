// esp32emu — cJSON tests
#include "cJSON.h"
#include <cassert>
#include <cstring>
#include <cstdio>

int main() {
    // Create object
    cJSON* root = cJSON_CreateObject();
    assert(cJSON_IsObject(root));

    cJSON_AddStringToObject(root, "name", "ESP32");
    cJSON_AddNumberToObject(root, "cores", 2);
    cJSON_AddBoolToObject(root, "wifi", 1);
    cJSON_AddNullToObject(root, "extra");

    // Query
    assert(cJSON_HasObjectItem(root, "name"));
    assert(!cJSON_HasObjectItem(root, "missing"));
    cJSON* name = cJSON_GetObjectItem(root, "name");
    assert(cJSON_IsString(name));
    assert(strcmp(cJSON_GetStringValue(name), "ESP32") == 0);
    cJSON* cores = cJSON_GetObjectItem(root, "cores");
    assert(cJSON_IsNumber(cores));
    assert(cores->valueint == 2);
    cJSON* wifi = cJSON_GetObjectItem(root, "wifi");
    assert(cJSON_IsTrue(wifi));

    // Print + parse roundtrip
    char* json = cJSON_PrintUnformatted(root);
    assert(json);
    cJSON* parsed = cJSON_Parse(json);
    assert(parsed);
    assert(cJSON_IsObject(parsed));
    assert(strcmp(cJSON_GetStringValue(cJSON_GetObjectItem(parsed, "name")), "ESP32") == 0);
    assert(cJSON_GetObjectItem(parsed, "cores")->valueint == 2);
    free(json);
    cJSON_Delete(parsed);

    // Array
    cJSON* arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, cJSON_CreateNumber(10));
    cJSON_AddItemToArray(arr, cJSON_CreateNumber(20));
    cJSON_AddItemToArray(arr, cJSON_CreateString("hello"));
    assert(cJSON_GetArraySize(arr) == 3);
    assert(cJSON_GetArrayItem(arr, 0)->valueint == 10);
    assert(cJSON_GetArrayItem(arr, 1)->valueint == 20);
    assert(strcmp(cJSON_GetStringValue(cJSON_GetArrayItem(arr, 2)), "hello") == 0);
    cJSON_Delete(arr);

    // Parse complex JSON
    const char* input = R"({"sensors":[{"id":1,"temp":22.5},{"id":2,"temp":23.1}],"ok":true})";
    cJSON* doc = cJSON_Parse(input);
    assert(doc);
    cJSON* sensors = cJSON_GetObjectItem(doc, "sensors");
    assert(cJSON_IsArray(sensors));
    assert(cJSON_GetArraySize(sensors) == 2);
    cJSON* s0 = cJSON_GetArrayItem(sensors, 0);
    assert(cJSON_GetObjectItem(s0, "id")->valueint == 1);
    assert(cJSON_GetObjectItem(s0, "temp")->valuedouble > 22.4);
    assert(cJSON_IsTrue(cJSON_GetObjectItem(doc, "ok")));

    // cJSON_ArrayForEach
    int count = 0;
    cJSON* elem = nullptr;
    cJSON_ArrayForEach(elem, sensors) { count++; }
    assert(count == 2);

    // Duplicate
    cJSON* dup = cJSON_Duplicate(doc, 1);
    assert(dup);
    assert(cJSON_GetArraySize(cJSON_GetObjectItem(dup, "sensors")) == 2);
    cJSON_Delete(dup);

    // Delete item
    cJSON_DeleteItemFromObject(doc, "ok");
    assert(!cJSON_HasObjectItem(doc, "ok"));

    // Replace item
    cJSON_ReplaceItemInObject(doc, "sensors", cJSON_CreateString("removed"));
    assert(cJSON_IsString(cJSON_GetObjectItem(doc, "sensors")));

    cJSON_Delete(doc);

    // Primitive arrays
    int nums[] = {1, 2, 3};
    cJSON* intArr = cJSON_CreateIntArray(nums, 3);
    assert(cJSON_GetArraySize(intArr) == 3);
    assert(cJSON_GetArrayItem(intArr, 2)->valueint == 3);
    cJSON_Delete(intArr);

    // Escape sequences in parse
    cJSON* esc = cJSON_Parse(R"({"msg":"hello\nworld"})");
    assert(esc);
    assert(strcmp(cJSON_GetStringValue(cJSON_GetObjectItem(esc, "msg")), "hello\nworld") == 0);
    cJSON_Delete(esc);

    cJSON_Delete(root);
    printf("test_cjson: all assertions passed\n");
    return 0;
}
