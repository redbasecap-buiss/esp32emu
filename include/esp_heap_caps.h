#pragma once
// esp32emu — esp_heap_caps mock
// Provides heap_caps_malloc/free/realloc/get_free_size and related functions.

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <atomic>

// Memory capability flags
#define MALLOC_CAP_EXEC         (1 << 0)
#define MALLOC_CAP_32BIT        (1 << 1)
#define MALLOC_CAP_8BIT         (1 << 2)
#define MALLOC_CAP_DMA          (1 << 3)
#define MALLOC_CAP_PID2         (1 << 4)
#define MALLOC_CAP_PID3         (1 << 5)
#define MALLOC_CAP_PID4         (1 << 6)
#define MALLOC_CAP_PID5         (1 << 7)
#define MALLOC_CAP_PID6         (1 << 8)
#define MALLOC_CAP_PID7         (1 << 9)
#define MALLOC_CAP_SPIRAM       (1 << 10)
#define MALLOC_CAP_INTERNAL     (1 << 11)
#define MALLOC_CAP_DEFAULT      (1 << 12)
#define MALLOC_CAP_IRAM_8BIT    (1 << 13)
#define MALLOC_CAP_RETENTION    (1 << 14)
#define MALLOC_CAP_RTCRAM       (1 << 15)

namespace esp32emu {
namespace heap {

struct HeapStats {
    std::atomic<size_t> total_allocated{0};
    size_t simulated_free = 320 * 1024;       // 320 KB simulated free heap
    size_t simulated_free_spiram = 4 * 1024 * 1024; // 4 MB PSRAM

    static HeapStats& instance() {
        static HeapStats s;
        return s;
    }
};

} // namespace heap
} // namespace esp32emu

inline void* heap_caps_malloc(size_t size, uint32_t /*caps*/) {
    void* p = malloc(size);
    if (p) esp32emu::heap::HeapStats::instance().total_allocated += size;
    return p;
}

inline void* heap_caps_calloc(size_t n, size_t size, uint32_t caps) {
    void* p = heap_caps_malloc(n * size, caps);
    if (p) memset(p, 0, n * size);
    return p;
}

inline void* heap_caps_realloc(void* ptr, size_t size, uint32_t /*caps*/) {
    return realloc(ptr, size);
}

inline void heap_caps_free(void* ptr) {
    free(ptr);
}

inline size_t heap_caps_get_free_size(uint32_t caps) {
    auto& stats = esp32emu::heap::HeapStats::instance();
    if (caps & MALLOC_CAP_SPIRAM) return stats.simulated_free_spiram;
    return stats.simulated_free;
}

inline size_t heap_caps_get_largest_free_block(uint32_t caps) {
    return heap_caps_get_free_size(caps) / 2;  // Simulate fragmentation
}

inline size_t heap_caps_get_minimum_free_size(uint32_t caps) {
    return heap_caps_get_free_size(caps) - 10240;  // Simulate some usage
}

inline size_t heap_caps_get_total_size(uint32_t caps) {
    if (caps & MALLOC_CAP_SPIRAM) return 4 * 1024 * 1024;
    return 512 * 1024;
}

// Multi-heap info structure
typedef struct {
    size_t total_free_bytes;
    size_t total_allocated_bytes;
    size_t largest_free_block;
    size_t minimum_free_bytes;
    size_t allocated_blocks;
    size_t free_blocks;
    size_t total_blocks;
} multi_heap_info_t;

inline void heap_caps_get_info(multi_heap_info_t* info, uint32_t caps) {
    if (!info) return;
    info->total_free_bytes = heap_caps_get_free_size(caps);
    info->total_allocated_bytes = esp32emu::heap::HeapStats::instance().total_allocated.load();
    info->largest_free_block = heap_caps_get_largest_free_block(caps);
    info->minimum_free_bytes = heap_caps_get_minimum_free_size(caps);
    info->allocated_blocks = 10;
    info->free_blocks = 5;
    info->total_blocks = 15;
}

// Test helper
namespace esp32emu {
namespace heap {
inline void setFreeSize(size_t internal, size_t spiram = 4 * 1024 * 1024) {
    HeapStats::instance().simulated_free = internal;
    HeapStats::instance().simulated_free_spiram = spiram;
}
} // namespace heap
}
