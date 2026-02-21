// esp32emu example — Monitor heap usage with heap_caps
#include <Arduino.h>
#include "esp_heap_caps.h"

void printHeapInfo(const char* label, uint32_t caps) {
    Serial.print(label);
    Serial.print(" — Free: ");
    Serial.print(heap_caps_get_free_size(caps));
    Serial.print(" B, Largest block: ");
    Serial.print(heap_caps_get_largest_free_block(caps));
    Serial.print(" B, Min ever: ");
    Serial.print(heap_caps_get_minimum_free_size(caps));
    Serial.println(" B");
}

void setup() {
    Serial.begin(115200);
    Serial.println("Heap Monitor Example");
    Serial.println("====================");

    printHeapInfo("Internal", MALLOC_CAP_INTERNAL);
    printHeapInfo("PSRAM   ", MALLOC_CAP_SPIRAM);
    printHeapInfo("DMA     ", MALLOC_CAP_DMA);
    printHeapInfo("Default ", MALLOC_CAP_DEFAULT);

    // Allocate some memory
    Serial.println("\nAllocating 4 KB from internal...");
    void* p = heap_caps_malloc(4096, MALLOC_CAP_INTERNAL);
    if (p) {
        Serial.println("Success!");
        printHeapInfo("Internal", MALLOC_CAP_INTERNAL);
        heap_caps_free(p);
    }

    // PSRAM allocation
    Serial.println("\nAllocating 64 KB from PSRAM...");
    void* ps = heap_caps_malloc(65536, MALLOC_CAP_SPIRAM);
    if (ps) {
        Serial.println("PSRAM allocation OK");
        heap_caps_free(ps);
    }

    // Detailed info
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
    Serial.println("\nDetailed heap info:");
    Serial.print("  Free bytes:      "); Serial.println(info.total_free_bytes);
    Serial.print("  Allocated bytes: "); Serial.println(info.total_allocated_bytes);
    Serial.print("  Free blocks:     "); Serial.println(info.free_blocks);
    Serial.print("  Total blocks:    "); Serial.println(info.total_blocks);
}

void loop() {
    delay(10000);
}
