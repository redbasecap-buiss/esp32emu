#include <Arduino.h>
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_err.h"

void setup() {
    Serial.begin(115200);
    Serial.println("SD/MMC Host Driver Demo");

    // Method 1: SDMMC (4-bit parallel)
    Serial.println("\n--- SDMMC Mode ---");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();

    esp_err_t ret = sdmmc_host_init();
    if (ret == ESP_OK) {
        Serial.println("SDMMC host initialized");
        sdmmc_host_init_slot(host.slot, &slot);
        sdmmc_host_set_bus_width(host.slot, 4);
        sdmmc_host_set_card_clk(host.slot, SDMMC_FREQ_HIGHSPEED);
        Serial.println("Slot configured: 4-bit, 40MHz");
        sdmmc_host_deinit();
        Serial.println("SDMMC host deinitialized");
    }

    // Method 2: SPI mode
    Serial.println("\n--- SPI Mode ---");
    sdspi_device_config_t dev_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_cfg.gpio_cs = 5;

    ret = sdspi_host_init();
    if (ret == ESP_OK) {
        Serial.println("SDSPI host initialized");
        sdspi_dev_handle_t handle;
        sdspi_host_init_device(&dev_cfg, &handle);
        Serial.print("Device handle: ");
        Serial.println(handle ? "valid" : "null");
        sdspi_host_remove_device(handle);
        sdspi_host_deinit();
        Serial.println("SDSPI host deinitialized");
    }
}

void loop() {
    delay(5000);
}
