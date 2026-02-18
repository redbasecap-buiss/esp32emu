#include "../include/esp32emu_board.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    using namespace esp32emu;

    // Test all board types exist and have valid configs
    auto& esp32 = getBoardConfig(BoardType::ESP32);
    assert(strcmp(esp32.name, "ESP32") == 0);
    assert(esp32.has_wifi && esp32.has_bluetooth);

    auto& c6 = getBoardConfig(BoardType::ESP32_C6);
    assert(strcmp(c6.name, "ESP32-C6") == 0);
    assert(c6.has_wifi && c6.has_bluetooth);
    assert(c6.cpu_freq_mhz == 160);
    assert(c6.digital_pins == 31);

    auto& pico = getBoardConfig(BoardType::RP2040);
    assert(strcmp(pico.name, "RP2040 Pico") == 0);
    assert(!pico.has_wifi && !pico.has_bluetooth);
    assert(pico.cpu_freq_mhz == 133);
    assert(pico.ram_bytes == 264000);
    assert(pico.builtin_led == 25);

    // Test new boards
    auto& h2 = getBoardConfig(BoardType::ESP32_H2);
    assert(strcmp(h2.name, "ESP32-H2") == 0);
    assert(!h2.has_wifi && h2.has_bluetooth);
    assert(h2.cpu_freq_mhz == 96);

    auto& teensy = getBoardConfig(BoardType::TEENSY40);
    assert(strcmp(teensy.name, "Teensy 4.0") == 0);
    assert(teensy.cpu_freq_mhz == 600);
    assert(teensy.ram_bytes == 1048576);
    assert(!teensy.has_wifi && !teensy.has_bluetooth);

    auto& bp = getBoardConfig(BoardType::STM32_BLUEPILL);
    assert(strcmp(bp.name, "STM32 Blue Pill") == 0);
    assert(bp.cpu_freq_mhz == 72);
    assert(!bp.has_wifi && !bp.has_bluetooth);

    // Test board name parsing
    assert(parseBoardName("esp32-c6") == BoardType::ESP32_C6);
    assert(parseBoardName("esp32c6") == BoardType::ESP32_C6);
    assert(parseBoardName("esp32-h2") == BoardType::ESP32_H2);
    assert(parseBoardName("esp32h2") == BoardType::ESP32_H2);
    assert(parseBoardName("pico") == BoardType::RP2040);
    assert(parseBoardName("rp2040") == BoardType::RP2040);
    assert(parseBoardName("teensy") == BoardType::TEENSY40);
    assert(parseBoardName("teensy40") == BoardType::TEENSY40);
    assert(parseBoardName("bluepill") == BoardType::STM32_BLUEPILL);
    assert(parseBoardName("stm32") == BoardType::STM32_BLUEPILL);
    assert(parseBoardName("uno") == BoardType::ARDUINO_UNO);
    assert(parseBoardName("unknown") == BoardType::ESP32); // default

    // Test Board singleton
    Board::instance().setBoard(BoardType::RP2040);
    assert(Board::instance().config().builtin_led == 25);
    assert(Board::instance().A0_base() == 26);

    Board::instance().setBoard(BoardType::ESP32_C6);
    assert(Board::instance().config().digital_pins == 31);
    assert(!Board::instance().checkWiFi() == false); // C6 has WiFi, so checkWiFi returns true

    // Reset
    Board::instance().setBoard(BoardType::ESP32);

    printf("test_boards: all assertions passed\n");
    return 0;
}
