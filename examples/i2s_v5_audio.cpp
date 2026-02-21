// ESP-IDF v5 I2S standard mode — audio output with new API
#include <Arduino.h>
#include <driver/i2s_std.h>
#include <cmath>

i2s_chan_handle_t tx_handle = nullptr;

void setup() {
    Serial.begin(115200);

    // Allocate channel
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(0, 0);
    i2s_new_channel(&chan_cfg, &tx_handle, nullptr);

    // Configure standard mode
    i2s_std_config_t std_cfg = {};
    std_cfg.clk_cfg.sample_rate_hz = 44100;
    std_cfg.clk_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;
    std_cfg.clk_cfg.slot_mode = I2S_SLOT_MODE_STEREO;
    std_cfg.gpio_cfg = {.mclk = -1, .bclk = 26, .ws = 25, .dout = 22, .din = -1};

    i2s_channel_init_std_mode(tx_handle, &std_cfg);
    i2s_channel_enable(tx_handle);
    Serial.println("I2S v5 audio output ready (44.1kHz stereo)");
}

void loop() {
    // Generate 440Hz sine wave (one period)
    const int samples = 100;
    int16_t buf[samples * 2]; // stereo
    for (int i = 0; i < samples; i++) {
        int16_t sample = (int16_t)(16000.0 * sin(2.0 * M_PI * i / samples));
        buf[i * 2] = sample;     // left
        buf[i * 2 + 1] = sample; // right
    }

    size_t written = 0;
    i2s_channel_write(tx_handle, buf, sizeof(buf), &written, 1000);
    delay(10);
}
