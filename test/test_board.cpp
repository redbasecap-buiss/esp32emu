#include "esp32emu_board.h"
#include <cassert>
#include <cstdio>
#include <cstring>

using namespace esp32emu;

int main() {
    // Test board parsing
    assert(parseBoardName("esp32") == BoardType::ESP32);
    assert(parseBoardName("esp32s3") == BoardType::ESP32_S3);
    assert(parseBoardName("esp32-s3") == BoardType::ESP32_S3);
    assert(parseBoardName("esp32c3") == BoardType::ESP32_C3);
    assert(parseBoardName("esp32-c3") == BoardType::ESP32_C3);
    assert(parseBoardName("uno") == BoardType::ARDUINO_UNO);
    assert(parseBoardName("mega") == BoardType::ARDUINO_MEGA);
    assert(parseBoardName("nano") == BoardType::ARDUINO_NANO);
    assert(parseBoardName("esp32s2") == BoardType::ESP32_S2);
    assert(parseBoardName("esp32-s2") == BoardType::ESP32_S2);
    assert(parseBoardName("unknown") == BoardType::ESP32); // default

    // Test ESP32 config
    auto& esp32 = getBoardConfig(BoardType::ESP32);
    assert(strcmp(esp32.name, "ESP32") == 0);
    assert(esp32.has_wifi == true);
    assert(esp32.has_bluetooth == true);
    assert(esp32.digital_pins == 40);
    assert(esp32.builtin_led == 2);

    // Test ESP32-C3 config
    auto& c3 = getBoardConfig(BoardType::ESP32_C3);
    assert(strcmp(c3.name, "ESP32-C3") == 0);
    assert(c3.has_wifi == true);
    assert(c3.has_bluetooth == false);
    assert(c3.digital_pins == 22);
    assert(c3.cpu_freq_mhz == 160);
    assert(c3.builtin_led == 8);

    // Test Arduino Uno config
    auto& uno = getBoardConfig(BoardType::ARDUINO_UNO);
    assert(strcmp(uno.chip, "ATmega328P") == 0);
    assert(uno.has_wifi == false);
    assert(uno.ram_bytes == 2048);
    assert(uno.builtin_led == 13);

    // Test Board singleton
    Board::instance().setBoard(BoardType::ESP32_C3);
    assert(Board::instance().config().digital_pins == 22);
    assert(Board::instance().checkWiFi() == true);

    Board::instance().setBoard(BoardType::ARDUINO_UNO);
    assert(Board::instance().A0_base() == 14);

    Board::instance().setBoard(BoardType::ARDUINO_MEGA);
    assert(Board::instance().A0_base() == 54);

    Board::instance().setBoard(BoardType::ESP32);
    assert(Board::instance().A0_base() == 36);

    // Test ESP32-S2 config
    auto& s2 = getBoardConfig(BoardType::ESP32_S2);
    assert(strcmp(s2.name, "ESP32-S2") == 0);
    assert(s2.has_wifi == true);
    assert(s2.has_bluetooth == false);
    assert(s2.digital_pins == 46);
    assert(s2.cpu_freq_mhz == 240);

    // Test Arduino Nano config
    auto& nano = getBoardConfig(BoardType::ARDUINO_NANO);
    assert(strcmp(nano.name, "Arduino Nano") == 0);
    assert(strcmp(nano.chip, "ATmega328P") == 0);
    assert(nano.has_wifi == false);
    assert(nano.analog_pins == 8);
    assert(nano.builtin_led == 13);

    Board::instance().setBoard(BoardType::ARDUINO_NANO);
    assert(Board::instance().A0_base() == 14);

    printf("test_board: all assertions passed\n");
    return 0;
}
