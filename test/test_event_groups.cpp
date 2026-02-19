// esp32emu test — FreeRTOS Event Groups
#include <cassert>
#include <cstdio>
#include <thread>
#include <chrono>
#include "freertos/event_groups.h"

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)

static void test_create_delete() {
    EventGroupHandle_t eg = xEventGroupCreate();
    assert(eg != nullptr);
    assert(xEventGroupGetBits(eg) == 0);
    vEventGroupDelete(eg);
}

static void test_set_get_clear() {
    EventGroupHandle_t eg = xEventGroupCreate();

    xEventGroupSetBits(eg, BIT_0 | BIT_2);
    assert(xEventGroupGetBits(eg) == (BIT_0 | BIT_2));

    EventBits_t prev = xEventGroupClearBits(eg, BIT_0);
    assert(prev == (BIT_0 | BIT_2));
    assert(xEventGroupGetBits(eg) == BIT_2);

    vEventGroupDelete(eg);
}

static void test_wait_any_bit() {
    EventGroupHandle_t eg = xEventGroupCreate();

    // Spawn thread that sets BIT_1 after a short delay
    std::thread t([eg]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        xEventGroupSetBits(eg, BIT_1);
    });

    EventBits_t bits = xEventGroupWaitBits(eg, BIT_0 | BIT_1, pdFALSE, pdFALSE, portMAX_DELAY);
    assert(bits & BIT_1);

    t.join();
    vEventGroupDelete(eg);
}

static void test_wait_all_bits() {
    EventGroupHandle_t eg = xEventGroupCreate();

    std::thread t1([eg]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        xEventGroupSetBits(eg, BIT_0);
    });
    std::thread t2([eg]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        xEventGroupSetBits(eg, BIT_1);
    });

    EventBits_t bits = xEventGroupWaitBits(eg, BIT_0 | BIT_1, pdFALSE, pdTRUE, portMAX_DELAY);
    assert((bits & (BIT_0 | BIT_1)) == (BIT_0 | BIT_1));

    t1.join();
    t2.join();
    vEventGroupDelete(eg);
}

static void test_wait_clear_on_exit() {
    EventGroupHandle_t eg = xEventGroupCreate();
    xEventGroupSetBits(eg, BIT_0 | BIT_1 | BIT_2);

    EventBits_t bits = xEventGroupWaitBits(eg, BIT_0 | BIT_1, pdTRUE, pdTRUE, 0);
    assert((bits & (BIT_0 | BIT_1)) == (BIT_0 | BIT_1));
    // BIT_0 and BIT_1 should be cleared, BIT_2 remains
    assert(xEventGroupGetBits(eg) == BIT_2);

    vEventGroupDelete(eg);
}

static void test_wait_timeout() {
    EventGroupHandle_t eg = xEventGroupCreate();

    auto start = std::chrono::steady_clock::now();
    EventBits_t bits = xEventGroupWaitBits(eg, BIT_0, pdFALSE, pdFALSE, 30);
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();

    assert((bits & BIT_0) == 0); // timed out, bit not set
    assert(elapsed >= 25);       // waited approximately 30ms

    vEventGroupDelete(eg);
}

static void test_sync() {
    EventGroupHandle_t eg = xEventGroupCreate();

    // Test sync: two threads set their bit and wait for both
    std::atomic<bool> t_done{false};
    std::thread t([eg, &t_done]() {
        // Set BIT_1, wait for BIT_0 | BIT_1 (no clear on exit for mock simplicity)
        xEventGroupSetBits(eg, BIT_1);
        EventBits_t bits = xEventGroupWaitBits(eg, BIT_0 | BIT_1, pdFALSE, pdTRUE, 1000);
        assert((bits & (BIT_0 | BIT_1)) == (BIT_0 | BIT_1));
        t_done = true;
    });

    // Give thread time to set BIT_1
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    xEventGroupSetBits(eg, BIT_0);

    // Wait for both
    EventBits_t bits = xEventGroupWaitBits(eg, BIT_0 | BIT_1, pdFALSE, pdTRUE, 1000);
    assert((bits & (BIT_0 | BIT_1)) == (BIT_0 | BIT_1));

    t.join();
    assert(t_done);
    vEventGroupDelete(eg);
}

int main() {
    test_create_delete();
    test_set_get_clear();
    test_wait_any_bit();
    test_wait_all_bits();
    test_wait_clear_on_exit();
    test_wait_timeout();
    test_sync();
    printf("test_event_groups: all assertions passed\n");
    return 0;
}
