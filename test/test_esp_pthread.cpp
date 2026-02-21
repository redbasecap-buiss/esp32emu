// esp32emu test — esp_pthread configuration
#include "esp_pthread.h"
#include <cassert>
#include <cstdio>
#include <cstring>

static void test_default_config() {
    esp32emu_pthread_test::reset();
    auto cfg = esp_pthread_get_default_config();
    assert(cfg.stack_size == 3072);
    assert(cfg.prio == 5);
    assert(cfg.inherit_cfg == false);
    assert(cfg.pin_to_core == -1);
}

static void test_set_get_config() {
    esp32emu_pthread_test::reset();
    esp_pthread_cfg_t cfg = {};
    cfg.stack_size = 8192;
    cfg.prio = 10;
    cfg.inherit_cfg = true;
    cfg.thread_name = "worker";
    cfg.pin_to_core = 1;
    assert(esp_pthread_set_cfg(&cfg) == ESP_OK);

    esp_pthread_cfg_t out = {};
    assert(esp_pthread_get_cfg(&out) == ESP_OK);
    assert(out.stack_size == 8192);
    assert(out.prio == 10);
    assert(out.pin_to_core == 1);
    assert(out.inherit_cfg == true);
}

static void test_unconfigured_returns_default() {
    esp32emu_pthread_test::reset();
    assert(!esp32emu_pthread_test::isConfigured());
    esp_pthread_cfg_t out = {};
    assert(esp_pthread_get_cfg(&out) == ESP_OK);
    assert(out.stack_size == 3072);
}

static void test_null_args() {
    assert(esp_pthread_set_cfg(nullptr) == -1);
    assert(esp_pthread_get_cfg(nullptr) == -1);
}

int main() {
    test_default_config();
    test_set_get_config();
    test_unconfigured_returns_default();
    test_null_args();
    printf("test_esp_pthread: all assertions passed\n");
    return 0;
}
