// Example: OTA firmware update using Update library
#include "Arduino.h"
#include "Update.h"

void setup() {
    Serial.begin(115200);
    Serial.println("OTA Update Example");

    // Simulate firmware update
    size_t firmware_size = 512;
    if (!Update.begin(firmware_size, U_FLASH)) {
        Serial.println("Update.begin() failed!");
        return;
    }
    Serial.println("Update started...");

    Update.onProgress([](size_t current, size_t total) {
        Serial.print("Progress: ");
        Serial.print((current * 100) / total);
        Serial.println("%");
    });

    // Write firmware data in chunks
    uint8_t chunk[128];
    memset(chunk, 0x42, sizeof(chunk));
    for (int i = 0; i < 4; i++) {
        Update.write(chunk, 128);
    }

    if (Update.end()) {
        Serial.println("Update successful!");
        if (Update.isFinished()) {
            Serial.println("Restarting...");
            ESP.restart();
        }
    } else {
        Serial.print("Update failed: ");
        Serial.println(Update.getErrorString());
    }
}

void loop() { delay(1000); }
