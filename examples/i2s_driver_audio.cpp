// ESP-IDF I2S driver example - audio output
#include "Arduino.h"
#include "driver/i2s.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void setup() {
    Serial.begin(115200);
    Serial.println("=== I2S Driver Audio Example ===");

    // Configure I2S
    i2s_config_t cfg{};
    cfg.mode = I2S_MODE_MASTER | I2S_MODE_TX;
    cfg.sample_rate = 44100;
    cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    cfg.dma_buf_count = 8;
    cfg.dma_buf_len = 64;

    esp_err_t err = i2s_driver_install(I2S_NUM_0, &cfg, 0, nullptr);
    Serial.print("I2S install: "); Serial.println(err == ESP_OK ? "OK" : "FAIL");

    // Set pins
    i2s_pin_config_t pins = {26, 25, 22, -1};
    i2s_set_pin(I2S_NUM_0, &pins);

    // Generate a 440Hz sine wave (one period)
    const int sample_rate = 44100;
    const float freq = 440.0f;
    const int samples_per_period = (int)(sample_rate / freq);

    int16_t buf[256];
    for (int i = 0; i < samples_per_period && i < 128; i++) {
        int16_t val = (int16_t)(16000 * sin(2.0 * M_PI * i / samples_per_period));
        buf[i * 2] = val;      // left
        buf[i * 2 + 1] = val;  // right
    }

    size_t written = 0;
    int count = (samples_per_period < 128 ? samples_per_period : 128);
    i2s_write(I2S_NUM_0, buf, count * 4, &written, 1000);
    Serial.print("Wrote "); Serial.print(written); Serial.println(" bytes of 440Hz sine");

    i2s_stop(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_0);
    Serial.println("Done.");
}

void loop() {
    delay(1000);
}
