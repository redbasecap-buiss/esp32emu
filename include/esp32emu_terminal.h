#pragma once
#include <cstdio>
#include <cstdint>
#include <string>
#include <map>
#include "esp32emu_gpio.h"

namespace esp32emu {

// ANSI color codes
#define TERM_RESET   "\033[0m"
#define TERM_RED     "\033[31m"
#define TERM_GREEN   "\033[32m"
#define TERM_YELLOW  "\033[33m"
#define TERM_BLUE    "\033[34m"
#define TERM_MAGENTA "\033[35m"
#define TERM_CYAN    "\033[36m"
#define TERM_WHITE   "\033[37m"
#define TERM_BOLD    "\033[1m"

// Pin state visualization
inline void printPinStates() {
    auto pins = GPIOController::instance().allPins();
    if (pins.empty()) return;

    fprintf(stderr, "\n" TERM_BOLD "┌─── GPIO States ───┐" TERM_RESET "\n");
    for (auto& [pin, state] : pins) {
        const char* mode_str = "IN ";
        if (state.mode == 0x1) mode_str = "OUT";
        else if (state.mode == 0x2) mode_str = "PUP";

        const char* color = state.digital ? TERM_GREEN : TERM_RED;
        fprintf(stderr, "│ GPIO %2d [%s] %s%s%s │\n",
                pin, mode_str, color, state.digital ? "HIGH ●" : "LOW  ○", TERM_RESET);
    }
    fprintf(stderr, TERM_BOLD "└────────────────────┘" TERM_RESET "\n");
}

// Colored serial output helper
inline void serialInfo(const char* msg) {
    fprintf(stdout, TERM_GREEN "[INFO] %s" TERM_RESET "\n", msg);
}

inline void serialWarn(const char* msg) {
    fprintf(stdout, TERM_YELLOW "[WARN] %s" TERM_RESET "\n", msg);
}

inline void serialError(const char* msg) {
    fprintf(stdout, TERM_RED "[ERROR] %s" TERM_RESET "\n", msg);
}

// NeoPixel-style RGB display in terminal
inline void printRGB(uint8_t r, uint8_t g, uint8_t b, int pixel = -1) {
    fprintf(stdout, "\033[48;2;%d;%d;%dm", r, g, b);
    if (pixel >= 0) fprintf(stdout, " %2d ", pixel);
    else fprintf(stdout, "    ");
    fprintf(stdout, TERM_RESET);
}

// LED visualization
inline void printLED(uint8_t pin, bool on) {
    if (on)
        fprintf(stderr, TERM_YELLOW "💡 LED (pin %d): ON  ████" TERM_RESET "\n", pin);
    else
        fprintf(stderr, "💡 LED (pin %d): OFF ░░░░\n", pin);
}

// Servo visualization
inline void printServo(int angle) {
    fprintf(stderr, "🔄 Servo: %3d° [", angle);
    int pos = angle * 40 / 180;
    for (int i = 0; i < 40; i++) {
        if (i == pos) fprintf(stderr, TERM_CYAN "█" TERM_RESET);
        else fprintf(stderr, "░");
    }
    fprintf(stderr, "]\n");
}

// LCD display visualization
inline void printLCD(int cols, int rows, const std::string lines[]) {
    fprintf(stderr, "\n┌");
    for (int i = 0; i < cols + 2; i++) fprintf(stderr, "─");
    fprintf(stderr, "┐\n");
    for (int r = 0; r < rows; r++) {
        fprintf(stderr, "│ " TERM_GREEN);
        int len = (int)lines[r].length();
        for (int c = 0; c < cols; c++) {
            if (c < len) fprintf(stderr, "%c", lines[r][c]);
            else fprintf(stderr, " ");
        }
        fprintf(stderr, TERM_RESET " │\n");
    }
    fprintf(stderr, "└");
    for (int i = 0; i < cols + 2; i++) fprintf(stderr, "─");
    fprintf(stderr, "┘\n");
}

} // namespace esp32emu
