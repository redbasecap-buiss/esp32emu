#include "ESPAsyncWebServer.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test 1: Basic route registration and handling
    AsyncWebServer server(8080);
    bool getCalled = false;
    bool postCalled = false;

    server.on("/hello", HTTP_GET, [&](AsyncWebServerRequest* req) {
        getCalled = true;
        req->send(200, "text/plain", "Hello World");
    });

    server.on("/data", HTTP_POST, [&](AsyncWebServerRequest* req) {
        postCalled = true;
        req->send(201, "application/json", "{\"ok\":true}");
    });

    server.begin();
    assert(server.isRunning());
    assert(server.routeCount() == 2);

    // Simulate GET /hello
    AsyncWebServerRequest req1;
    req1.setMethod("GET");
    req1.setUrl("/hello");
    assert(server.simulateRequest(&req1));
    assert(getCalled);
    assert(req1.responded());
    assert(req1.lastResponse().code() == 200);
    assert(req1.lastResponse().body() == "Hello World");

    // Simulate POST /data
    AsyncWebServerRequest req2;
    req2.setMethod("POST");
    req2.setUrl("/data");
    req2.setBody("{\"value\":42}");
    assert(server.simulateRequest(&req2));
    assert(postCalled);
    assert(req2.lastResponse().code() == 201);

    // Test 2: Not found handler
    bool notFoundCalled = false;
    server.onNotFound([&](AsyncWebServerRequest* req) {
        notFoundCalled = true;
        req->send(404, "text/plain", "Not Found");
    });

    AsyncWebServerRequest req3;
    req3.setMethod("GET");
    req3.setUrl("/nonexistent");
    server.simulateRequest(&req3);
    assert(notFoundCalled);
    assert(req3.lastResponse().code() == 404);

    // Test 3: Query params
    AsyncWebServerRequest req4;
    req4.setMethod("GET");
    req4.setUrl("/hello");
    req4.setParam("name", "esp32");
    assert(req4.hasParam("name"));
    assert(req4.getParam("name") == "esp32");
    assert(!req4.hasParam("missing"));

    // Test 4: Headers
    AsyncWebServerRequest req5;
    req5.setHeader("Content-Type", "application/json");
    assert(req5.hasHeader("Content-Type"));
    assert(req5.header("Content-Type") == "application/json");

    // Test 5: Redirect
    server.on("/old", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->redirect("/new");
    });
    AsyncWebServerRequest req6;
    req6.setMethod("GET");
    req6.setUrl("/old");
    server.simulateRequest(&req6);
    assert(req6.lastResponse().code() == 302);

    // Test 6: Method mismatch
    AsyncWebServerRequest req7;
    req7.setMethod("DELETE");
    req7.setUrl("/hello");
    // /hello is GET only, so notFound should be called
    notFoundCalled = false;
    server.simulateRequest(&req7);
    assert(notFoundCalled);

    server.end();
    assert(!server.isRunning());

    printf("test_asyncwebserver: all assertions passed\n");
    return 0;
}
