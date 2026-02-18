#include "Ticker.h"
#include <cassert>
#include <atomic>
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    // Test 1: attach and fire
    {
        std::atomic<int> count{0};
        Ticker t;
        t.attach_ms(50, [&]() { count++; });
        assert(t.active());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        t.detach();
        assert(!t.active());
        assert(count >= 2); // should fire multiple times in 350ms
        printf("ticker attach: count=%d\n", count.load());
    }

    // Test 2: once
    {
        std::atomic<int> count{0};
        Ticker t;
        t.once_ms(50, [&]() { count++; });
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        assert(count == 1);
        assert(!t.active());
        printf("ticker once: count=%d\n", count.load());
    }

    // Test 3: detach stops firing
    {
        std::atomic<int> count{0};
        Ticker t;
        t.attach_ms(30, [&]() { count++; });
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        t.detach();
        int snapshot = count;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        assert(count == snapshot); // no more fires after detach
    }

    // Test 4: reattach replaces previous
    {
        std::atomic<int> a{0}, b{0};
        Ticker t;
        t.attach_ms(30, [&]() { a++; });
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        t.attach_ms(30, [&]() { b++; });
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        t.detach();
        assert(b >= 1);
        int a_snap = a;
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        assert(a == a_snap); // old callback stopped
    }

    printf("test_ticker: all assertions passed\n");
    return 0;
}
