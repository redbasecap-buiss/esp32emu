// esp32emu — ESP HTTPS OTA mock tests
#include "esp_https_ota.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test 1: simple one-shot OTA
    {
        esp32emu::OtaState::instance().reset();
        esp_http_client_config_t http_cfg = {};
        http_cfg.url = "https://example.com/firmware.bin";
        esp_https_ota_config_t cfg = {};
        cfg.http_config = http_cfg;
        assert(esp_https_ota(&cfg) == ESP_OK);
        assert(esp32emu::OtaState::instance().completed);
    }

    // Test 2: one-shot OTA failure
    {
        esp32emu::OtaState::instance().reset();
        esp32emu::OtaState::instance().shouldFail = true;
        esp_https_ota_config_t cfg = {};
        cfg.http_config.url = "https://example.com/bad.bin";
        assert(esp_https_ota(&cfg) == ESP_FAIL);
        assert(!esp32emu::OtaState::instance().completed);
    }

    // Test 3: begin/perform/finish with progress
    {
        esp32emu::OtaState::instance().reset();
        esp_https_ota_config_t cfg = {};
        cfg.http_config.url = "https://example.com/fw.bin";
        esp_https_ota_handle_t handle = nullptr;

        assert(esp_https_ota_begin(&cfg, &handle) == ESP_OK);
        assert(handle != nullptr);

        // Perform downloads in chunks (totalSize/4 per call)
        esp_err_t err = esp_https_ota_perform(handle);
        assert(err == ESP_ERR_HTTPS_OTA_IN_PROGRESS);
        assert(esp_https_ota_get_image_len_read(handle) > 0);

        err = esp_https_ota_perform(handle);
        assert(err == ESP_ERR_HTTPS_OTA_IN_PROGRESS);

        err = esp_https_ota_perform(handle);
        assert(err == ESP_ERR_HTTPS_OTA_IN_PROGRESS);

        err = esp_https_ota_perform(handle);
        assert(err == ESP_OK);  // Complete
        assert(esp_https_ota_is_complete_data_received(handle));
        assert(esp_https_ota_get_image_len_read(handle) == esp_https_ota_get_image_size(handle));

        assert(esp_https_ota_finish(handle) == ESP_OK);
        assert(esp32emu::OtaState::instance().completed);
    }

    // Test 4: begin failure
    {
        esp32emu::OtaState::instance().reset();
        esp32emu::OtaState::instance().shouldFail = true;
        esp_https_ota_config_t cfg = {};
        cfg.http_config.url = "https://fail.com/fw.bin";
        esp_https_ota_handle_t handle = nullptr;
        assert(esp_https_ota_begin(&cfg, &handle) == ESP_FAIL);
    }

    printf("test_https_ota: all assertions passed\n");
    return 0;
}
