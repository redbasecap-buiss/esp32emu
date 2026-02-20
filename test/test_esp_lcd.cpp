// Test: ESP-IDF LCD panel driver mock
#include "esp_lcd.h"
#include <cassert>
#include <cstdio>

int main() {
    // Create SPI IO
    esp_lcd_panel_io_spi_config_t io_cfg = {};
    io_cfg.cs_gpio_num = 5;
    io_cfg.dc_gpio_num = 16;
    io_cfg.pclk_hz = 40000000;

    esp_lcd_panel_io_handle_t io = nullptr;
    assert(esp_lcd_new_panel_io_spi(nullptr, &io_cfg, &io) == ESP_OK);
    assert(io != nullptr);
    assert(io->cs_gpio == 5);
    assert(io->initialized);

    // Create ST7789 panel
    esp_lcd_panel_dev_config_t panel_cfg = {};
    panel_cfg.reset_gpio_num = 4;
    panel_cfg.bits_per_pixel = 16;

    esp_lcd_panel_handle_t panel = nullptr;
    assert(esp_lcd_new_panel_st7789(io, &panel_cfg, &panel) == ESP_OK);
    assert(panel != nullptr);
    assert(panel->width == 240);
    assert(panel->height == 320);

    // Reset and init
    assert(esp_lcd_panel_reset(panel) == ESP_OK);
    assert(panel->reset_done);
    assert(esp_lcd_panel_init(panel) == ESP_OK);
    assert(panel->initialized);
    assert(!panel->framebuffer.empty());

    // Display on
    assert(esp_lcd_panel_disp_on_off(panel, true) == ESP_OK);
    assert(panel->display_on);

    // Mirror/swap
    assert(esp_lcd_panel_mirror(panel, true, false) == ESP_OK);
    assert(panel->mirror_x && !panel->mirror_y);
    assert(esp_lcd_panel_swap_xy(panel, true) == ESP_OK);
    assert(panel->swap_xy);

    // Gap
    assert(esp_lcd_panel_set_gap(panel, 0, 20) == ESP_OK);
    assert(panel->gap_y == 20);

    // Invert color
    assert(esp_lcd_panel_invert_color(panel, true) == ESP_OK);
    assert(panel->invert_color);

    // Draw bitmap
    uint16_t pixels[4] = {0xF800, 0x07E0, 0x001F, 0xFFFF};
    assert(esp_lcd_panel_draw_bitmap(panel, 0, 0, 2, 2, pixels) == ESP_OK);
    // Verify pixel data in framebuffer
    assert(panel->framebuffer[0] == 0x00);  // 0xF800 low byte
    assert(panel->framebuffer[1] == 0xF8);  // 0xF800 high byte

    // IO tx
    uint8_t param = 0x36;
    assert(esp_lcd_panel_io_tx_param(io, 0, &param, 1) == ESP_OK);
    assert(io->tx_data.back() == 0x36);

    // Create SSD1306 panel via I2C
    esp_lcd_panel_io_i2c_config_t i2c_cfg = {};
    i2c_cfg.dev_addr = 0x3C;
    esp_lcd_panel_io_handle_t i2c_io = nullptr;
    assert(esp_lcd_new_panel_io_i2c(nullptr, &i2c_cfg, &i2c_io) == ESP_OK);

    esp_lcd_panel_dev_config_t oled_cfg = {};
    oled_cfg.bits_per_pixel = 1;
    esp_lcd_panel_handle_t oled = nullptr;
    assert(esp_lcd_new_panel_ssd1306(i2c_io, &oled_cfg, &oled) == ESP_OK);
    assert(oled->width == 128 && oled->height == 64);

    // Cleanup
    esp_lcd_panel_del(panel);
    esp_lcd_panel_io_del(io);
    esp_lcd_panel_del(oled);
    esp_lcd_panel_io_del(i2c_io);

    printf("test_esp_lcd: all assertions passed\n");
    return 0;
}
