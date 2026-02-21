#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include <cassert>
#include <cstdio>

int main() {
    printf("test_sdmmc_host: ");

    // SDMMC host init/deinit
    assert(sdmmc_host_init() == ESP_OK);
    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    assert(sdmmc_host_init_slot(1, &slot) == ESP_OK);
    assert(sdmmc_host_set_bus_width(1, 4) == ESP_OK);
    assert(sdmmc_host_set_card_clk(1, SDMMC_FREQ_HIGHSPEED) == ESP_OK);
    assert(sdmmc_host_deinit() == ESP_OK);

    // SDMMC host default config
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    assert(host.flags == SDMMC_HOST_FLAG_4BIT);
    assert(host.max_freq_khz == SDMMC_FREQ_DEFAULT);

    // SDSPI host
    assert(sdspi_host_init() == ESP_OK);
    sdspi_device_config_t dev_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
    assert(dev_cfg.gpio_cs == 5);
    sdspi_dev_handle_t handle;
    assert(sdspi_host_init_device(&dev_cfg, &handle) == ESP_OK);
    assert(handle != nullptr);
    assert(sdspi_host_remove_device(handle) == ESP_OK);
    assert(sdspi_host_deinit() == ESP_OK);

    printf("all assertions passed\n");
    return 0;
}
