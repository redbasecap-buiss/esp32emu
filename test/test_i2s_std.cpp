// esp32emu test — ESP-IDF v5 I2S standard mode driver
#include "driver/i2s_std.h"
#include <cassert>
#include <cstdio>
#include <cstring>

static void test_new_channel() {
    esp32emu_i2s_std_test::reset();
    i2s_chan_handle_t tx = nullptr, rx = nullptr;
    i2s_chan_config_t cfg = {.id = 0, .dma_desc_num = 6, .dma_frame_num = 240, .auto_clear = true};
    assert(i2s_new_channel(&cfg, &tx, &rx) == ESP_OK);
    assert(tx != nullptr);
    assert(rx != nullptr);
}

static void test_init_std_mode() {
    esp32emu_i2s_std_test::reset();
    i2s_chan_handle_t tx = nullptr, rx = nullptr;
    i2s_chan_config_t chan_cfg = {.id = 0, .dma_desc_num = 6, .dma_frame_num = 240, .auto_clear = true};
    i2s_new_channel(&chan_cfg, &tx, &rx);

    i2s_std_config_t std_cfg = {};
    std_cfg.clk_cfg.sample_rate_hz = 48000;
    std_cfg.clk_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;
    std_cfg.clk_cfg.slot_mode = I2S_SLOT_MODE_STEREO;
    assert(i2s_channel_init_std_mode(tx, &std_cfg) == ESP_OK);
    assert(esp32emu_i2s_std_test::getSampleRate() == 48000);
}

static void test_enable_disable() {
    esp32emu_i2s_std_test::reset();
    i2s_chan_handle_t tx = nullptr, rx = nullptr;
    i2s_chan_config_t cfg = {.id = 0, .dma_desc_num = 6, .dma_frame_num = 240, .auto_clear = true};
    i2s_new_channel(&cfg, &tx, &rx);

    assert(!esp32emu_i2s_std_test::isTxEnabled());
    assert(i2s_channel_enable(tx) == ESP_OK);
    assert(esp32emu_i2s_std_test::isTxEnabled());
    assert(i2s_channel_disable(tx) == ESP_OK);
    assert(!esp32emu_i2s_std_test::isTxEnabled());
}

static void test_write_read() {
    esp32emu_i2s_std_test::reset();
    i2s_chan_handle_t tx = nullptr, rx = nullptr;
    i2s_chan_config_t cfg = {.id = 0, .dma_desc_num = 6, .dma_frame_num = 240, .auto_clear = true};
    i2s_new_channel(&cfg, &tx, &rx);
    i2s_channel_enable(tx);
    i2s_channel_enable(rx);

    // Write TX data
    uint8_t txdata[] = {0xAA, 0xBB, 0xCC, 0xDD};
    size_t written = 0;
    assert(i2s_channel_write(tx, txdata, 4, &written, 100) == ESP_OK);
    assert(written == 4);
    auto buf = esp32emu_i2s_std_test::getTxBuffer();
    assert(buf.size() == 4);
    assert(buf[0] == 0xAA && buf[3] == 0xDD);

    // Inject and read RX data
    uint8_t inject[] = {0x11, 0x22, 0x33};
    esp32emu_i2s_std_test::injectRxData(inject, 3);
    uint8_t rxdata[4] = {};
    size_t rd = 0;
    assert(i2s_channel_read(rx, rxdata, 4, &rd, 100) == ESP_OK);
    assert(rd == 3);
    assert(rxdata[0] == 0x11 && rxdata[2] == 0x33);
}

static void test_delete_channel() {
    esp32emu_i2s_std_test::reset();
    i2s_chan_handle_t tx = nullptr, rx = nullptr;
    i2s_chan_config_t cfg = {.id = 0, .dma_desc_num = 6, .dma_frame_num = 240, .auto_clear = true};
    i2s_new_channel(&cfg, &tx, &rx);
    i2s_channel_enable(tx);
    i2s_del_channel(tx);
    assert(!esp32emu_i2s_std_test::isTxEnabled());
}

int main() {
    test_new_channel();
    test_init_std_mode();
    test_enable_disable();
    test_write_read();
    test_delete_channel();
    printf("test_i2s_std: all assertions passed\n");
    return 0;
}
