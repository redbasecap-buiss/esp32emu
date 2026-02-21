#pragma once
#include "esp_err.h"
#include <cstdint>
#include <cstddef>

// ESP-IDF SDMMC host driver mock

#define SDMMC_HOST_FLAG_1BIT   (1 << 0)
#define SDMMC_HOST_FLAG_4BIT   (1 << 1)
#define SDMMC_HOST_FLAG_8BIT   (1 << 2)
#define SDMMC_HOST_FLAG_DDR    (1 << 3)

#define SDMMC_FREQ_DEFAULT     20000
#define SDMMC_FREQ_HIGHSPEED   40000
#define SDMMC_FREQ_PROBING     400

typedef int sdmmc_host_t_slot;

typedef struct {
    uint32_t flags;
    int slot;
    int max_freq_khz;
    float io_voltage;
    esp_err_t (*init)(void);
    esp_err_t (*set_bus_width)(int slot, size_t width);
    esp_err_t (*set_card_clk)(int slot, uint32_t freq_khz);
    esp_err_t (*do_transaction)(int slot, void* cmdinfo);
    esp_err_t (*deinit)(void);
    int command_timeout_ms;
} sdmmc_host_t;

#define SDMMC_HOST_DEFAULT() { \
    .flags = SDMMC_HOST_FLAG_4BIT, \
    .slot = 1, \
    .max_freq_khz = SDMMC_FREQ_DEFAULT, \
    .io_voltage = 3.3f, \
    .init = nullptr, \
    .set_bus_width = nullptr, \
    .set_card_clk = nullptr, \
    .do_transaction = nullptr, \
    .deinit = nullptr, \
    .command_timeout_ms = 0 \
}

typedef struct {
    int clk;
    int cmd;
    int d0;
    int d1;
    int d2;
    int d3;
    int d4;
    int d5;
    int d6;
    int d7;
    int cd;
    int wp;
    int width;
    int flags;
} sdmmc_slot_config_t;

#define SDMMC_SLOT_CONFIG_DEFAULT() { \
    .clk = 14, .cmd = 15, .d0 = 2, .d1 = 4, .d2 = 12, .d3 = 13, \
    .d4 = -1, .d5 = -1, .d6 = -1, .d7 = -1, \
    .cd = -1, .wp = -1, .width = 0, .flags = 0 \
}

inline esp_err_t sdmmc_host_init(void) { return ESP_OK; }
inline esp_err_t sdmmc_host_init_slot(int slot, const sdmmc_slot_config_t* cfg) {
    (void)slot; (void)cfg; return ESP_OK;
}
inline esp_err_t sdmmc_host_deinit(void) { return ESP_OK; }
inline esp_err_t sdmmc_host_set_bus_width(int slot, size_t width) {
    (void)slot; (void)width; return ESP_OK;
}
inline esp_err_t sdmmc_host_set_card_clk(int slot, uint32_t freq_khz) {
    (void)slot; (void)freq_khz; return ESP_OK;
}
