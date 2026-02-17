#pragma once
#include <cstdint>
#include <cstdio>
#include <string>

namespace esp32emu {

enum class BoardType {
    ESP32,
    ESP32_S3,
    ARDUINO_UNO,
    ARDUINO_MEGA
};

struct BoardConfig {
    BoardType type;
    const char* name;
    const char* chip;
    int digital_pins;
    int analog_pins;
    int ram_bytes;
    int flash_bytes;
    int cpu_freq_mhz;
    bool has_wifi;
    bool has_bluetooth;
    int builtin_led;
};

inline const BoardConfig& getBoardConfig(BoardType t) {
    static const BoardConfig configs[] = {
        {BoardType::ESP32,       "ESP32",        "ESP32",       40, 8,  327680,  4194304, 240, true,  true,  2},
        {BoardType::ESP32_S3,    "ESP32-S3",     "ESP32-S3",    48, 10, 524288,  8388608, 240, true,  true,  2},
        {BoardType::ARDUINO_UNO, "Arduino Uno",  "ATmega328P",  14, 6,  2048,    32768,   16,  false, false, 13},
        {BoardType::ARDUINO_MEGA,"Arduino Mega", "ATmega2560",  54, 16, 8192,    262144,  16,  false, false, 13},
    };
    return configs[static_cast<int>(t)];
}

inline BoardType parseBoardName(const std::string& name) {
    if (name == "uno")    return BoardType::ARDUINO_UNO;
    if (name == "mega")   return BoardType::ARDUINO_MEGA;
    if (name == "esp32s3" || name == "esp32-s3") return BoardType::ESP32_S3;
    return BoardType::ESP32; // default
}

class Board {
public:
    static Board& instance() {
        static Board b;
        return b;
    }

    void setBoard(BoardType t) { config_ = &getBoardConfig(t); }
    const BoardConfig& config() const { return *config_; }

    bool validatePin(uint8_t pin) const {
        if (pin >= config_->digital_pins && pin < A0_base()) {
            fprintf(stderr, "\033[33m[esp32emu] WARNING: Pin %d exceeds %s digital pin count (%d)\033[0m\n",
                    pin, config_->name, config_->digital_pins);
            return false;
        }
        return true;
    }

    bool checkWiFi() const {
        if (!config_->has_wifi) {
            fprintf(stderr, "\033[33m[esp32emu] WARNING: %s does not have WiFi! Call ignored.\033[0m\n",
                    config_->name);
            return false;
        }
        return true;
    }

    int A0_base() const {
        switch (config_->type) {
            case BoardType::ARDUINO_UNO:  return 14; // A0=14..A5=19
            case BoardType::ARDUINO_MEGA: return 54; // A0=54..A15=69
            default: return 36; // ESP32
        }
    }

    void printBoardInfo() const {
        fprintf(stderr, "[esp32emu] Board: %s | Chip: %s | RAM: %dB | Flash: %dB | CPU: %dMHz\n",
                config_->name, config_->chip, config_->ram_bytes, config_->flash_bytes, config_->cpu_freq_mhz);
        fprintf(stderr, "[esp32emu] Digital Pins: %d | Analog Pins: %d | WiFi: %s | LED_BUILTIN: %d\n",
                config_->digital_pins, config_->analog_pins,
                config_->has_wifi ? "Yes" : "No", config_->builtin_led);
    }

    void printASCII() const {
        switch (config_->type) {
            case BoardType::ARDUINO_UNO: printUnoASCII(); break;
            case BoardType::ARDUINO_MEGA: printMegaASCII(); break;
            case BoardType::ESP32_S3: printESP32S3ASCII(); break;
            default: printESP32ASCII(); break;
        }
    }

private:
    const BoardConfig* config_ = &getBoardConfig(BoardType::ESP32);

    void printUnoASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║         ARDUINO UNO              ║
    ║         ATmega328P               ║
    ║                                  ║
    ║  D0-D13    [■■■■■■■■■■■■■■]     ║
    ║  A0-A5     [■■■■■■]             ║
    ║  PWR  [●]  USB [═══]            ║
    ╚══════════════════════════════════╝
)");
    }

    void printMegaASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════════════════╗
    ║              ARDUINO MEGA 2560               ║
    ║              ATmega2560                       ║
    ║                                              ║
    ║  D0-D53    [■■■■■■■■■■■■■■■■■■■■■■■■■■■]   ║
    ║  A0-A15    [■■■■■■■■■■■■■■■■]               ║
    ║  PWR  [●]  USB [═══]                         ║
    ╚══════════════════════════════════════════════╝
)");
    }

    void printESP32ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║           ESP32-WROOM            ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ WiFi/BT    ║
    ║          └──────────┘            ║
    ║  GPIO 0-39  [■■■■■■■■■■■■]     ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printESP32S3ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║          ESP32-S3-WROOM          ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ WiFi/BT5   ║
    ║          └──────────┘            ║
    ║  GPIO 0-48  [■■■■■■■■■■■■■■]   ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }
};

} // namespace esp32emu
