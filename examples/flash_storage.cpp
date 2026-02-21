// esp32emu example — ESP-IDF SPI Flash read/write/erase
#include <Arduino.h>
#include <esp_flash.h>

void setup() {
    Serial.begin(115200);
    Serial.println("=== SPI Flash Storage Demo ===");

    // Get flash size
    uint32_t flash_size;
    esp_flash_get_size(nullptr, &flash_size);
    Serial.printf("Flash size: %u bytes (%.1f MB)\n", flash_size, flash_size / 1048576.0f);

    // Erase a sector
    uint32_t addr = 0x100000; // 1MB offset
    esp_flash_erase_region(nullptr, addr, 4096);
    Serial.printf("Erased sector at 0x%06X\n", addr);

    // Write data
    const char* msg = "Hello from ESP32 flash!";
    esp_flash_write(nullptr, msg, addr, strlen(msg) + 1);
    Serial.printf("Wrote: \"%s\"\n", msg);

    // Read back
    char buf[64] = {};
    esp_flash_read(nullptr, buf, addr, sizeof(buf));
    Serial.printf("Read:  \"%s\"\n", buf);
    Serial.println(strcmp(msg, buf) == 0 ? "✅ Match!" : "❌ Mismatch!");

    // Demonstrate flash AND behavior
    Serial.println("\n--- Flash AND behavior ---");
    esp_flash_erase_region(nullptr, addr + 4096, 4096);
    uint8_t first[4] = {0xFF, 0xF0, 0x0F, 0x00};
    esp_flash_write(nullptr, first, addr + 4096, 4);
    uint8_t second[4] = {0x0F, 0xFF, 0xFF, 0xFF};
    esp_flash_write(nullptr, second, addr + 4096, 4);
    uint8_t result[4];
    esp_flash_read(nullptr, result, addr + 4096, 4);
    Serial.printf("After two writes: %02X %02X %02X %02X\n",
                  result[0], result[1], result[2], result[3]);
    Serial.println("(Flash can only clear bits without erase)");
}

void loop() { delay(10000); }
