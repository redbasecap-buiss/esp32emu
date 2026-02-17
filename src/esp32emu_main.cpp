// esp32emu — main() that calls user's setup() and loop()
#include "Arduino.h"
#include "WiFi.h"
#include "esp32emu_board.h"
#include <csignal>
#include <cstdio>
#include <cstring>
#include <atomic>

// User must define these
extern void setup();
extern void loop();

static std::atomic<bool> running{true};

static void sighandler(int sig) {
    (void)sig;
    fprintf(stderr, "\n[esp32emu] Interrupted — shutting down.\n");
    running = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    // Parse --board option
    esp32emu::BoardType board = esp32emu::BoardType::ESP32;
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "--board") == 0 || strcmp(argv[i], "-b") == 0) && i + 1 < argc) {
            board = esp32emu::parseBoardName(argv[++i]);
        }
    }

    esp32emu::Board::instance().setBoard(board);
    auto& cfg = esp32emu::Board::instance().config();

    // Wire up WiFi check for non-WiFi boards
    if (!cfg.has_wifi) {
        WiFi.setWiFiCheck([]() {
            fprintf(stderr, "\033[33m[esp32emu] WARNING: This board does not have WiFi! Call ignored.\033[0m\n");
            return false;
        });
    }

    fprintf(stderr, "[esp32emu] Starting emulator...\n");
    esp32emu::Board::instance().printASCII();
    esp32emu::Board::instance().printBoardInfo();
    fprintf(stderr, "[esp32emu] Press Ctrl+C to stop.\n\n");

    setup();

    while (running) {
        loop();
    }

    fprintf(stderr, "[esp32emu] Stopped.\n");
    return 0;
}
