#pragma once
// esp32emu — ESP32 LEDC PWM controller mock with full state tracking
// Replaces the inline stubs in Arduino.h with proper channel state.

#include <cstdint>
#include <cmath>
#include <array>
#include "esp32emu_gpio.h"

namespace esp32emu {

struct LEDCChannel {
    bool configured = false;
    double frequency = 0;
    uint8_t resolution_bits = 0;
    uint32_t duty = 0;
    int8_t pin = -1;  // attached pin (-1 = none)
    double tone_freq = 0;
};

class LEDCController {
public:
    static constexpr int MAX_CHANNELS = 16;

    static LEDCController& instance() {
        static LEDCController ctrl;
        return ctrl;
    }

    double setup(uint8_t channel, double freq, uint8_t resolution_bits) {
        if (channel >= MAX_CHANNELS) return 0;
        auto& ch = channels_[channel];
        ch.configured = true;
        ch.frequency = freq;
        ch.resolution_bits = resolution_bits;
        ch.duty = 0;
        return freq;
    }

    void attachPin(uint8_t pin, uint8_t channel) {
        if (channel >= MAX_CHANNELS) return;
        channels_[channel].pin = pin;
    }

    void detachPin(uint8_t pin) {
        for (auto& ch : channels_) {
            if (ch.pin == (int8_t)pin) ch.pin = -1;
        }
    }

    void write(uint8_t channel, uint32_t duty) {
        if (channel >= MAX_CHANNELS) return;
        channels_[channel].duty = duty;
        if (channels_[channel].pin >= 0) {
            GPIOController::instance().analogWrite(channels_[channel].pin, duty);
        }
    }

    uint32_t read(uint8_t channel) const {
        if (channel >= MAX_CHANNELS) return 0;
        return channels_[channel].duty;
    }

    double readFreq(uint8_t channel) const {
        if (channel >= MAX_CHANNELS) return 0;
        return channels_[channel].frequency;
    }

    void writeTone(uint8_t channel, double freq) {
        if (channel >= MAX_CHANNELS) return;
        channels_[channel].tone_freq = freq;
        channels_[channel].frequency = freq;
        // Set 50% duty
        uint32_t max_duty = (1 << channels_[channel].resolution_bits) / 2;
        channels_[channel].duty = freq > 0 ? max_duty : 0;
    }

    void writeNote(uint8_t channel, int note, int octave) {
        // MIDI note to frequency: A4=440Hz
        double freq = 440.0 * std::pow(2.0, (note - 9) / 12.0 + (octave - 4));
        writeTone(channel, freq);
    }

    const LEDCChannel& getChannel(uint8_t channel) const {
        static LEDCChannel dummy;
        if (channel >= MAX_CHANNELS) return dummy;
        return channels_[channel];
    }

    void reset() {
        for (auto& ch : channels_) ch = LEDCChannel{};
    }

private:
    std::array<LEDCChannel, MAX_CHANNELS> channels_;
};

} // namespace esp32emu

// Free functions matching Arduino/ESP32 API
inline double ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits) {
    return esp32emu::LEDCController::instance().setup(channel, freq, resolution_bits);
}
inline void ledcAttachPin(uint8_t pin, uint8_t channel) {
    esp32emu::LEDCController::instance().attachPin(pin, channel);
}
inline void ledcDetachPin(uint8_t pin) {
    esp32emu::LEDCController::instance().detachPin(pin);
}
inline void ledcWrite(uint8_t channel, uint32_t duty) {
    esp32emu::LEDCController::instance().write(channel, duty);
}
inline uint32_t ledcRead(uint8_t channel) {
    return esp32emu::LEDCController::instance().read(channel);
}
inline double ledcReadFreq(uint8_t channel) {
    return esp32emu::LEDCController::instance().readFreq(channel);
}
inline void ledcWriteTone(uint8_t channel, double freq) {
    esp32emu::LEDCController::instance().writeTone(channel, freq);
}
inline void ledcWriteNote(uint8_t channel, int note, int octave) {
    esp32emu::LEDCController::instance().writeNote(channel, note, octave);
}

// Test helpers
namespace ledc_test {
    inline void reset() { esp32emu::LEDCController::instance().reset(); }
    inline const esp32emu::LEDCChannel& getChannel(uint8_t ch) {
        return esp32emu::LEDCController::instance().getChannel(ch);
    }
    inline bool isConfigured(uint8_t ch) { return getChannel(ch).configured; }
    inline uint32_t getDuty(uint8_t ch) { return getChannel(ch).duty; }
    inline double getFrequency(uint8_t ch) { return getChannel(ch).frequency; }
    inline int8_t getPin(uint8_t ch) { return getChannel(ch).pin; }
}
