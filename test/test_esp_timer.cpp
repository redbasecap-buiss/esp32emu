// esp32emu — test_esp_timer: ESP-IDF high-resolution timer test
#include "esp_timer.h"
#include <cassert>
#include <cstdio>
#include <atomic>
#include <thread>
#include <chrono>

static std::atomic<int> counter{0};

static void timer_callback(void* arg) {
    (void)arg;
    counter++;
}

int main() {
    // Test esp_timer_get_time returns increasing values
    int64_t t1 = esp_timer_get_time();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int64_t t2 = esp_timer_get_time();
    assert(t2 > t1);
    assert((t2 - t1) >= 5000); // at least 5ms in microseconds

    // Test periodic timer
    counter = 0;
    esp_timer_create_args_t args = {};
    args.callback = timer_callback;
    args.name = "test_periodic";

    esp_timer_handle_t timer = nullptr;
    int ret = esp_timer_create(&args, &timer);
    assert(ret == ESP_OK);
    assert(timer != nullptr);
    assert(!esp_timer_is_active(timer));

    ret = esp_timer_start_periodic(timer, 10000); // 10ms
    assert(ret == ESP_OK);
    assert(esp_timer_is_active(timer));

    std::this_thread::sleep_for(std::chrono::milliseconds(75));
    esp_timer_stop(timer);
    int count_periodic = counter.load();
    assert(count_periodic >= 3); // should fire ~7 times in 75ms, at least 3

    // Test one-shot timer
    counter = 0;
    esp_timer_handle_t oneshot = nullptr;
    esp_timer_create_args_t args2 = {};
    args2.callback = timer_callback;
    args2.name = "test_oneshot";
    ret = esp_timer_create(&args2, &oneshot);
    assert(ret == ESP_OK);

    ret = esp_timer_start_once(oneshot, 15000); // 15ms
    assert(ret == ESP_OK);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    assert(counter.load() == 1); // fires exactly once
    assert(!esp_timer_is_active(oneshot));

    // Test fire count helper
    assert(esp_timer_test::getFireCount(oneshot) == 1);
    esp_timer_test::resetFireCount(oneshot);
    assert(esp_timer_test::getFireCount(oneshot) == 0);

    // Cleanup
    esp_timer_delete(timer);
    esp_timer_delete(oneshot);

    // Test invalid args
    assert(esp_timer_create(nullptr, &timer) == ESP_ERR_INVALID_ARG);
    assert(esp_timer_stop(nullptr) == ESP_ERR_INVALID_ARG);
    assert(esp_timer_delete(nullptr) == ESP_ERR_INVALID_ARG);

    printf("test_esp_timer: all assertions passed\n");
    return 0;
}
