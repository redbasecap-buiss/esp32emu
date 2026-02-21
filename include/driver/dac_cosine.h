#pragma once
// esp32emu — ESP-IDF DAC cosine wave generator mock
// Generates cosine waveform output on DAC channels with configurable frequency,
// amplitude, phase offset, and DC offset.

#include <cstdint>
#include <cstdio>
#include <cmath>
#include <mutex>

#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_ERR_INVALID_ARG
#define ESP_ERR_INVALID_ARG 0x102
#endif

typedef void* dac_cosine_handle_t;

typedef enum {
    DAC_COSINE_ATTEN_DEFAULT = 0,   // No attenuation (full scale)
    DAC_COSINE_ATTEN_DB_0  = 0,
    DAC_COSINE_ATTEN_DB_6  = 1,     // 1/2 amplitude
    DAC_COSINE_ATTEN_DB_12 = 2,     // 1/4 amplitude
    DAC_COSINE_ATTEN_DB_18 = 3      // 1/8 amplitude
} dac_cosine_atten_t;

typedef enum {
    DAC_COSINE_PHASE_0   = 0,
    DAC_COSINE_PHASE_180 = 2
} dac_cosine_phase_t;

typedef struct {
    int chan_id;                    // DAC channel (0 or 1)
    uint32_t freq_hz;              // Frequency in Hz
    bool clk_src_8m;               // Use 8MHz clock (vs RTC)
    dac_cosine_atten_t atten;      // Amplitude attenuation
    dac_cosine_phase_t phase;      // Phase offset
    int8_t offset;                 // DC offset (-128 to 127)
} dac_cosine_config_t;

namespace esp32emu {
namespace dac_cosine {

struct CosineState {
    bool allocated = false;
    bool started = false;
    dac_cosine_config_t config = {};

    static CosineState& instance() {
        static CosineState s;
        return s;
    }
    void reset() { *this = CosineState(); }

    // Calculate expected voltage at a given time point (for testing)
    double voltageAt(double time_s) const {
        if (!started) return 0.0;
        double amplitude = 127.0;
        switch (config.atten) {
            case DAC_COSINE_ATTEN_DB_6:  amplitude = 63.5; break;
            case DAC_COSINE_ATTEN_DB_12: amplitude = 31.75; break;
            case DAC_COSINE_ATTEN_DB_18: amplitude = 15.875; break;
            default: break;
        }
        double phase = (config.phase == DAC_COSINE_PHASE_180) ? M_PI : 0.0;
        return amplitude * cos(2.0 * M_PI * config.freq_hz * time_s + phase) + 128.0 + config.offset;
    }
};

} // namespace dac_cosine
} // namespace esp32emu

inline int dac_cosine_new_channel(const dac_cosine_config_t* cfg, dac_cosine_handle_t* handle) {
    if (!cfg || !handle) return ESP_ERR_INVALID_ARG;
    auto& s = esp32emu::dac_cosine::CosineState::instance();
    s.config = *cfg;
    s.allocated = true;
    *handle = &s;
    printf("[esp32emu] DAC cosine: channel %d, freq=%uHz, atten=%d\n",
           cfg->chan_id, cfg->freq_hz, cfg->atten);
    return ESP_OK;
}

inline int dac_cosine_start(dac_cosine_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    auto& s = *static_cast<esp32emu::dac_cosine::CosineState*>(handle);
    s.started = true;
    printf("[esp32emu] DAC cosine: started\n");
    return ESP_OK;
}

inline int dac_cosine_stop(dac_cosine_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    auto& s = *static_cast<esp32emu::dac_cosine::CosineState*>(handle);
    s.started = false;
    return ESP_OK;
}

inline int dac_cosine_del_channel(dac_cosine_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    auto& s = *static_cast<esp32emu::dac_cosine::CosineState*>(handle);
    s.started = false;
    s.allocated = false;
    printf("[esp32emu] DAC cosine: channel deleted\n");
    return ESP_OK;
}

// Test helpers
namespace esp32emu_dac_cosine_test {
    inline bool isStarted() {
        return esp32emu::dac_cosine::CosineState::instance().started;
    }
    inline uint32_t getFrequency() {
        return esp32emu::dac_cosine::CosineState::instance().config.freq_hz;
    }
    inline int getChannel() {
        return esp32emu::dac_cosine::CosineState::instance().config.chan_id;
    }
    inline dac_cosine_atten_t getAttenuation() {
        return esp32emu::dac_cosine::CosineState::instance().config.atten;
    }
    inline double voltageAt(double t) {
        return esp32emu::dac_cosine::CosineState::instance().voltageAt(t);
    }
    inline void reset() {
        esp32emu::dac_cosine::CosineState::instance().reset();
    }
}
