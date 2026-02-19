#include "driver/i2s.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    esp32emu::i2s_driver::reset();

    // Install driver
    i2s_config_t cfg{};
    cfg.mode = I2S_MODE_MASTER | I2S_MODE_TX;
    cfg.sample_rate = 44100;
    cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    cfg.dma_buf_count = 8;
    cfg.dma_buf_len = 64;

    assert(i2s_driver_install(I2S_NUM_0, &cfg, 0, nullptr) == ESP_OK);
    assert(esp32emu::i2s_driver::port(I2S_NUM_0).installed);

    // Set pins
    i2s_pin_config_t pins = {26, 25, 22, -1};
    assert(i2s_set_pin(I2S_NUM_0, &pins) == ESP_OK);
    assert(esp32emu::i2s_driver::port(I2S_NUM_0).pins.bck_io_num == 26);

    // Set sample rate
    assert(i2s_set_sample_rates(I2S_NUM_0, 48000) == ESP_OK);
    assert(esp32emu::i2s_driver::port(I2S_NUM_0).config.sample_rate == 48000);

    // Write
    int16_t samples[4] = {100, -200, 300, -400};
    size_t written = 0;
    assert(i2s_write(I2S_NUM_0, samples, sizeof(samples), &written, 100) == ESP_OK);
    assert(written == sizeof(samples));
    assert(esp32emu::i2s_driver::port(I2S_NUM_0).tx_data.size() == sizeof(samples));

    // Read with injection
    {
        auto& p = esp32emu::i2s_driver::port(I2S_NUM_0);
        uint8_t inject[] = {0xAA, 0xBB, 0xCC, 0xDD};
        p.rx_inject.assign(inject, inject + 4);
        p.rx_pos = 0;
    }
    uint8_t rbuf[4] = {};
    size_t bread = 0;
    assert(i2s_read(I2S_NUM_0, rbuf, 4, &bread, 100) == ESP_OK);
    assert(bread == 4);
    assert(rbuf[0] == 0xAA);
    assert(rbuf[3] == 0xDD);

    // Start/stop
    assert(i2s_start(I2S_NUM_0) == ESP_OK);
    assert(i2s_stop(I2S_NUM_0) == ESP_OK);
    assert(i2s_zero_dma_buffer(I2S_NUM_0) == ESP_OK);

    // Uninstall
    assert(i2s_driver_uninstall(I2S_NUM_0) == ESP_OK);
    assert(!esp32emu::i2s_driver::port(I2S_NUM_0).installed);

    // Invalid port
    assert(i2s_driver_install(-1, &cfg, 0, nullptr) == ESP_FAIL);

    printf("test_i2s_driver: ALL PASSED\n");
    return 0;
}
