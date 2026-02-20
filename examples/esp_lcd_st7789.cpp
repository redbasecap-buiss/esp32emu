// ESP-IDF LCD panel driver example (ST7789)
#include <Arduino.h>
#include <esp_lcd.h>

void setup() {
    Serial.begin(115200);

    // Create SPI IO
    esp_lcd_panel_io_spi_config_t io_cfg = {};
    io_cfg.cs_gpio_num = 5;
    io_cfg.dc_gpio_num = 16;
    io_cfg.pclk_hz = 40000000;

    esp_lcd_panel_io_handle_t io;
    esp_lcd_new_panel_io_spi(nullptr, &io_cfg, &io);

    // Create ST7789 panel
    esp_lcd_panel_dev_config_t panel_cfg = {};
    panel_cfg.reset_gpio_num = 4;
    panel_cfg.bits_per_pixel = 16;

    esp_lcd_panel_handle_t panel;
    esp_lcd_new_panel_st7789(io, &panel_cfg, &panel);

    esp_lcd_panel_reset(panel);
    esp_lcd_panel_init(panel);
    esp_lcd_panel_invert_color(panel, true);
    esp_lcd_panel_disp_on_off(panel, true);

    // Draw red rectangle
    uint16_t red[100];
    for (int i = 0; i < 100; i++) red[i] = 0xF800;
    esp_lcd_panel_draw_bitmap(panel, 70, 110, 80, 120, red);

    Serial.println("ST7789 LCD initialized and drawn");
}

void loop() {
    delay(1000);
}
