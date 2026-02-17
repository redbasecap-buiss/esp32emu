#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include "esp32emu_terminal.h"

class LiquidCrystal {
public:
    LiquidCrystal(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
        (void)rs; (void)en; (void)d4; (void)d5; (void)d6; (void)d7;
    }

    void begin(uint8_t cols, uint8_t rows) {
        cols_ = cols;
        rows_ = rows;
        lines_ = new std::string[rows];
        for (int i = 0; i < rows; i++) lines_[i] = "";
        fprintf(stderr, "[esp32emu] LCD initialized: %dx%d\n", cols, rows);
    }

    void clear() {
        for (int i = 0; i < rows_; i++) lines_[i] = "";
        col_ = 0; row_ = 0;
        refresh();
    }

    void home() { col_ = 0; row_ = 0; }

    void setCursor(uint8_t col, uint8_t row) {
        col_ = col;
        row_ = row;
    }

    size_t print(const char* s) {
        if (!s || row_ >= rows_) return 0;
        for (const char* p = s; *p; p++) {
            if (col_ < cols_) {
                if ((int)lines_[row_].length() <= col_)
                    lines_[row_].resize(col_ + 1, ' ');
                lines_[row_][col_] = *p;
                col_++;
            }
        }
        refresh();
        return strlen(s);
    }

    size_t print(const std::string& s) { return print(s.c_str()); }
    size_t print(int n) { return print(std::to_string(n).c_str()); }
    size_t print(long n) { return print(std::to_string(n).c_str()); }
    size_t print(double n, int dec = 2) {
        char buf[32]; snprintf(buf, sizeof(buf), "%.*f", dec, n);
        return print(buf);
    }
    size_t print(char c) { char s[2] = {c, 0}; return print(s); }

    void display() {}
    void noDisplay() {}
    void scrollDisplayLeft() {}
    void scrollDisplayRight() {}
    void autoscroll() {}
    void noAutoscroll() {}
    void blink() {}
    void noBlink() {}
    void cursor() {}
    void noCursor() {}

    ~LiquidCrystal() { delete[] lines_; }

private:
    int cols_ = 16, rows_ = 2;
    int col_ = 0, row_ = 0;
    std::string* lines_ = nullptr;

    void refresh() {
        if (lines_) esp32emu::printLCD(cols_, rows_, lines_);
    }
};
