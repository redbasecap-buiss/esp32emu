// HX711 Load Cell Scale example
#include "Arduino.h"
#include "HX711.h"
#include <cstdio>

#define DOUT_PIN 4
#define SCK_PIN  5

HX711 scale;

void setup() {
    Serial.begin(115200);
    scale.begin(DOUT_PIN, SCK_PIN);

    // Simulate calibration
    scale.setRawValue(50000);   // empty scale reading
    scale.tare();               // zero out
    scale.set_scale(420.0f);    // calibration factor

    Serial.println("Scale ready. Place item...");

    // Simulate 500g weight
    scale.setRawValue(260000);  // (260000 - 50000) / 420 ≈ 500g
}

void loop() {
    if (scale.is_ready()) {
        float weight = scale.get_units(10);
        printf("Weight: %.1f g\n", weight);
    }
    delay(500);
}
