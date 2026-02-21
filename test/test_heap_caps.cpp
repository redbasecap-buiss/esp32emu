// esp32emu — heap_caps tests
#include "esp_heap_caps.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Basic alloc/free
    void* p = heap_caps_malloc(128, MALLOC_CAP_DEFAULT);
    assert(p != nullptr);
    memset(p, 0xAA, 128);
    heap_caps_free(p);

    // Calloc (zeroed)
    uint8_t* cp = (uint8_t*)heap_caps_calloc(10, 4, MALLOC_CAP_8BIT);
    assert(cp != nullptr);
    for (int i = 0; i < 40; i++) assert(cp[i] == 0);
    heap_caps_free(cp);

    // Realloc
    void* r = heap_caps_malloc(64, MALLOC_CAP_INTERNAL);
    assert(r != nullptr);
    r = heap_caps_realloc(r, 256, MALLOC_CAP_INTERNAL);
    assert(r != nullptr);
    heap_caps_free(r);

    // Free size queries
    size_t internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    assert(internal_free > 0);
    size_t spiram_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    assert(spiram_free > internal_free);

    size_t largest = heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT);
    assert(largest > 0 && largest <= internal_free);

    size_t total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    assert(total > 0);

    // Heap info struct
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
    assert(info.total_free_bytes > 0);
    assert(info.total_blocks > 0);

    // Test helper: set free size
    esp32emu::heap::setFreeSize(1024, 2048);
    assert(heap_caps_get_free_size(MALLOC_CAP_INTERNAL) == 1024);
    assert(heap_caps_get_free_size(MALLOC_CAP_SPIRAM) == 2048);

    // DMA allocation
    void* dma = heap_caps_malloc(512, MALLOC_CAP_DMA);
    assert(dma != nullptr);
    heap_caps_free(dma);

    printf("test_heap_caps: all assertions passed\n");
    return 0;
}
