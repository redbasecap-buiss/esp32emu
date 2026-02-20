// random_generator.cpp — ESP-IDF hardware RNG example
#include <Arduino.h>
#include "esp_random.h"

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF Random Number Generator");
    Serial.println("================================");

    // Generate random 32-bit numbers
    for (int i = 0; i < 5; i++) {
        Serial.printf("  esp_random(): 0x%08X\n", esp_random());
    }

    // Fill buffer with random bytes
    uint8_t key[16];
    esp_fill_random(key, sizeof(key));
    Serial.print("\n  Random key: ");
    for (size_t i = 0; i < sizeof(key); i++) {
        Serial.printf("%02X", key[i]);
    }
    Serial.println();

    // Generate random in range [0, max)
    uint32_t dice = (esp_random() % 6) + 1;
    Serial.printf("\n  🎲 Dice roll: %u\n", dice);
}

void loop() {
    delay(2000);
    Serial.printf("Random: 0x%08X\n", esp_random());
}
