#pragma once
#include "esp_err.h"
#include <cstdint>

// ESP-IDF SD SPI host driver mock

typedef void* sdspi_dev_handle_t;

typedef struct {
    int host_id;        // SPI host (SPI2_HOST, SPI3_HOST)
    int gpio_cs;        // Chip select GPIO
    int gpio_cd;        // Card detect GPIO (-1 = unused)
    int gpio_wp;        // Write protect GPIO (-1 = unused)
    int gpio_int;       // Interrupt GPIO (-1 = unused)
    int dma_channel;
} sdspi_device_config_t;

#define SDSPI_DEVICE_CONFIG_DEFAULT() { \
    .host_id = 1, \
    .gpio_cs = 5, \
    .gpio_cd = -1, \
    .gpio_wp = -1, \
    .gpio_int = -1, \
    .dma_channel = 1 \
}

typedef struct {
    uint32_t flags;
    int slot;
    int max_freq_khz;
    float io_voltage;
    esp_err_t (*init)(void);
    esp_err_t (*deinit)(void);
    int command_timeout_ms;
} sdspi_host_t;

#define SDSPI_HOST_DEFAULT() { \
    .flags = 0, \
    .slot = 1, \
    .max_freq_khz = 20000, \
    .io_voltage = 3.3f, \
    .init = nullptr, \
    .deinit = nullptr, \
    .command_timeout_ms = 0 \
}

inline esp_err_t sdspi_host_init(void) { return ESP_OK; }
inline esp_err_t sdspi_host_init_device(const sdspi_device_config_t* cfg, sdspi_dev_handle_t* handle) {
    (void)cfg;
    if (handle) *handle = (void*)1;
    return ESP_OK;
}
inline esp_err_t sdspi_host_remove_device(sdspi_dev_handle_t handle) {
    (void)handle; return ESP_OK;
}
inline esp_err_t sdspi_host_deinit(void) { return ESP_OK; }
