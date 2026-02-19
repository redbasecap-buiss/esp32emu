#include "esp_ota_ops.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    esp32emu::ota::reset();
    esp32emu::partition::reset();

    auto* running = esp_ota_get_running_partition();
    assert(running != nullptr);
    assert(strcmp(running->label, "factory") == 0);

    auto* next = esp_ota_get_next_update_partition(nullptr);
    assert(next != nullptr);
    assert(strcmp(next->label, "ota_0") == 0);

    esp_ota_handle_t handle;
    esp_err_t err = esp_ota_begin(next, 0, &handle);
    assert(err == ESP_OK);

    const char* data = "firmware_data_here";
    err = esp_ota_write(handle, data, strlen(data));
    assert(err == ESP_OK);

    auto& s = esp32emu::ota::state();
    assert(s.written_data.size() == strlen(data));

    err = esp_ota_end(handle);
    assert(err == ESP_OK);
    assert(!s.in_progress);

    err = esp_ota_set_boot_partition(next);
    assert(err == ESP_OK);
    assert(s.set_boot_called);

    auto* boot = esp_ota_get_boot_partition();
    assert(boot == next);

    esp_app_desc_t desc;
    err = esp_ota_get_partition_description(running, &desc);
    assert(err == ESP_OK);
    assert(strcmp(desc.version, "1.0.0") == 0);

    printf("test_esp_ota: ALL PASSED\n");
    return 0;
}
