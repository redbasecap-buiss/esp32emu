// esp32emu – ESP-IDF LCD panel driver mock (esp_lcd)
#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
#include <functional>

#ifndef ESP_OK
#define ESP_OK   0
#define ESP_FAIL -1
#endif
#ifndef ESP_ERR_INVALID_ARG
#define ESP_ERR_INVALID_ARG 0x102
#endif
typedef int esp_err_t;

// LCD panel I/O handle
struct esp_lcd_panel_io_t {
    int cs_gpio = -1;
    int dc_gpio = -1;
    int sclk_speed_hz = 0;
    bool initialized = false;
    std::vector<uint8_t> tx_data;
};
typedef esp_lcd_panel_io_t* esp_lcd_panel_io_handle_t;

// LCD panel handle
struct esp_lcd_panel_t {
    int width = 0;
    int height = 0;
    bool reset_done = false;
    bool initialized = false;
    bool display_on = false;
    bool mirror_x = false;
    bool mirror_y = false;
    bool swap_xy = false;
    int gap_x = 0;
    int gap_y = 0;
    int color_bits = 16;
    bool invert_color = false;
    std::vector<uint8_t> framebuffer;
    esp_lcd_panel_io_handle_t io = nullptr;
};
typedef esp_lcd_panel_t* esp_lcd_panel_handle_t;

// SPI panel IO config
struct esp_lcd_panel_io_spi_config_t {
    int cs_gpio_num = -1;
    int dc_gpio_num = -1;
    int spi_mode = 0;
    int pclk_hz = 10000000;
    size_t trans_queue_depth = 10;
    void* user_ctx = nullptr;
};

// I2C panel IO config
struct esp_lcd_panel_io_i2c_config_t {
    uint8_t dev_addr = 0x3C;
    int control_phase_bytes = 1;
    int dc_bit_offset = 6;
    int lcd_cmd_bits = 8;
    int lcd_param_bits = 8;
};

// Panel dev config
struct esp_lcd_panel_dev_config_t {
    int reset_gpio_num = -1;
    int color_space = 0;   // 0=RGB, 1=BGR
    int bits_per_pixel = 16;
};

// Panel IO operations
inline esp_err_t esp_lcd_new_panel_io_spi(void* bus, const esp_lcd_panel_io_spi_config_t* cfg,
                                           esp_lcd_panel_io_handle_t* ret) {
    auto io = new esp_lcd_panel_io_t();
    io->cs_gpio = cfg->cs_gpio_num;
    io->dc_gpio = cfg->dc_gpio_num;
    io->sclk_speed_hz = cfg->pclk_hz;
    io->initialized = true;
    *ret = io;
    return ESP_OK;
}

inline esp_err_t esp_lcd_new_panel_io_i2c(void* bus, const esp_lcd_panel_io_i2c_config_t* cfg,
                                           esp_lcd_panel_io_handle_t* ret) {
    (void)cfg;
    auto io = new esp_lcd_panel_io_t();
    io->initialized = true;
    *ret = io;
    (void)bus;
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io) {
    delete io;
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_io_tx_param(esp_lcd_panel_io_handle_t io, int cmd,
                                            const void* param, size_t param_size) {
    (void)cmd;
    if (param && param_size > 0) {
        auto p = static_cast<const uint8_t*>(param);
        io->tx_data.insert(io->tx_data.end(), p, p + param_size);
    }
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_io_tx_color(esp_lcd_panel_io_handle_t io, int cmd,
                                            const void* color, size_t color_size) {
    return esp_lcd_panel_io_tx_param(io, cmd, color, color_size);
}

// ST7789 panel
inline esp_err_t esp_lcd_new_panel_st7789(esp_lcd_panel_io_handle_t io,
                                           const esp_lcd_panel_dev_config_t* cfg,
                                           esp_lcd_panel_handle_t* ret) {
    auto panel = new esp_lcd_panel_t();
    panel->width = 240;
    panel->height = 320;
    panel->color_bits = cfg->bits_per_pixel;
    panel->io = io;
    *ret = panel;
    return ESP_OK;
}

// SSD1306 panel
inline esp_err_t esp_lcd_new_panel_ssd1306(esp_lcd_panel_io_handle_t io,
                                            const esp_lcd_panel_dev_config_t* cfg,
                                            esp_lcd_panel_handle_t* ret) {
    auto panel = new esp_lcd_panel_t();
    panel->width = 128;
    panel->height = 64;
    panel->color_bits = cfg->bits_per_pixel;
    panel->io = io;
    *ret = panel;
    return ESP_OK;
}

// Panel operations
inline esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t panel) {
    panel->reset_done = true;
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t panel) {
    panel->initialized = true;
    panel->framebuffer.resize(panel->width * panel->height * (panel->color_bits / 8), 0);
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t panel) {
    delete panel;
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_draw_bitmap(esp_lcd_panel_handle_t panel,
                                            int x_start, int y_start, int x_end, int y_end,
                                            const void* color_data) {
    if (!panel->initialized) return ESP_FAIL;
    int bpp = panel->color_bits / 8;
    int w = x_end - x_start;
    auto src = static_cast<const uint8_t*>(color_data);
    for (int y = y_start; y < y_end && y < panel->height; y++) {
        for (int x = x_start; x < x_end && x < panel->width; x++) {
            int dst_off = (y * panel->width + x) * bpp;
            int src_off = ((y - y_start) * w + (x - x_start)) * bpp;
            for (int b = 0; b < bpp; b++)
                panel->framebuffer[dst_off + b] = src[src_off + b];
        }
    }
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_mirror(esp_lcd_panel_handle_t panel, bool mirror_x, bool mirror_y) {
    panel->mirror_x = mirror_x;
    panel->mirror_y = mirror_y;
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_swap_xy(esp_lcd_panel_handle_t panel, bool swap) {
    panel->swap_xy = swap;
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_set_gap(esp_lcd_panel_handle_t panel, int x, int y) {
    panel->gap_x = x;
    panel->gap_y = y;
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_invert_color(esp_lcd_panel_handle_t panel, bool invert) {
    panel->invert_color = invert;
    return ESP_OK;
}

inline esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t panel, bool on) {
    panel->display_on = on;
    return ESP_OK;
}

// Alias for older API
inline esp_err_t esp_lcd_panel_disp_off(esp_lcd_panel_handle_t panel, bool off) {
    return esp_lcd_panel_disp_on_off(panel, !off);
}
