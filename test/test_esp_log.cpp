// esp32emu — test_esp_log: ESP-IDF logging macros test
#include "esp_log.h"
#include <cassert>
#include <string>
#include <vector>

static const char* TAG = "test";

int main() {
    // Reset state
    esp_log_test::reset();

    // Test basic logging and counting
    ESP_LOGE(TAG, "error %d", 1);
    ESP_LOGW(TAG, "warning %d", 2);
    ESP_LOGI(TAG, "info %d", 3);
    // DEBUG and VERBOSE should be filtered at default INFO level
    ESP_LOGD(TAG, "debug %d", 4);
    ESP_LOGV(TAG, "verbose %d", 5);

    assert(esp_log_test::getCount(ESP_LOG_ERROR) == 1);
    assert(esp_log_test::getCount(ESP_LOG_WARN) == 1);
    assert(esp_log_test::getCount(ESP_LOG_INFO) == 1);
    assert(esp_log_test::getCount(ESP_LOG_DEBUG) == 0);  // filtered
    assert(esp_log_test::getCount(ESP_LOG_VERBOSE) == 0); // filtered

    // Test level change
    esp_log_test::reset();
    esp_log_test::setLevel(ESP_LOG_VERBOSE);
    ESP_LOGD(TAG, "now visible");
    ESP_LOGV(TAG, "also visible");
    assert(esp_log_test::getCount(ESP_LOG_DEBUG) == 1);
    assert(esp_log_test::getCount(ESP_LOG_VERBOSE) == 1);

    // Test callback
    esp_log_test::reset();
    std::vector<std::string> captured;
    esp_log_test::setCallback([&](esp_log_level_t lvl, const char* tag, const char* msg) {
        (void)lvl;
        captured.push_back(std::string(tag) + ": " + msg);
    });
    ESP_LOGI("app", "hello %s", "world");
    assert(captured.size() == 1);
    assert(captured[0] == "app: hello world");

    // Test NONE level suppresses everything
    esp_log_test::reset();
    esp_log_test::setLevel(ESP_LOG_NONE);
    ESP_LOGE(TAG, "should not count");
    assert(esp_log_test::getCount(ESP_LOG_ERROR) == 0);

    // Test esp_log_level_set
    esp_log_test::reset();
    esp_log_level_set("*", ESP_LOG_DEBUG);
    ESP_LOGD(TAG, "debug after level_set");
    assert(esp_log_test::getCount(ESP_LOG_DEBUG) == 1);

    printf("test_esp_log: all assertions passed\n");
    return 0;
}
