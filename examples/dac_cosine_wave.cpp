// DAC cosine wave generator — output sine/cosine waveform on DAC pin
#include <Arduino.h>
#include <driver/dac_cosine.h>

dac_cosine_handle_t dac_handle = nullptr;

void setup() {
    Serial.begin(115200);

    dac_cosine_config_t cos_cfg = {};
    cos_cfg.chan_id = 0;          // DAC channel 1 (GPIO25)
    cos_cfg.freq_hz = 1000;       // 1 kHz
    cos_cfg.atten = DAC_COSINE_ATTEN_DB_0;  // Full scale
    cos_cfg.phase = DAC_COSINE_PHASE_0;
    cos_cfg.offset = 0;

    dac_cosine_new_channel(&cos_cfg, &dac_handle);
    dac_cosine_start(dac_handle);

    Serial.println("DAC cosine: 1kHz waveform on GPIO25");
}

void loop() {
    // Waveform runs autonomously in hardware
    // Sweep frequency
    static uint32_t freq = 1000;
    freq += 100;
    if (freq > 5000) freq = 100;

    Serial.print("Frequency: ");
    Serial.print(freq);
    Serial.println(" Hz");

    delay(2000);
}
