// tls_client.cpp — ESP-TLS client example
// Demonstrates using the esp_tls API for (mock) TLS connections

#include <Arduino.h>
#include "esp_tls.h"

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-TLS Client Example");

    // Configure TLS
    esp_tls_cfg_t cfg = {};
    cfg.skip_common_name = true;

    // Connect to host
    Serial.println("Connecting to example.com:443...");
    esp_tls_t* tls = esp_tls_conn_new("example.com", strlen("example.com"), 443, &cfg);

    if (!tls) {
        Serial.println("❌ Connection failed!");
        return;
    }
    Serial.println("✅ Connected!");

    // Send HTTP request
    const char* request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    ssize_t written = esp_tls_conn_write(tls, request, strlen(request));
    Serial.printf("📤 Sent %zd bytes\n", written);

    // Read response
    char buf[256];
    ssize_t rd = esp_tls_conn_read(tls, buf, sizeof(buf) - 1);
    if (rd > 0) {
        buf[rd] = '\0';
        Serial.printf("📥 Received %zd bytes: %s\n", rd, buf);
    } else {
        Serial.println("📥 No data (mock EOF)");
    }

    // Cleanup
    esp_tls_conn_destroy(tls);
    Serial.println("🔒 Connection closed");
}

void loop() {
    delay(5000);
}
