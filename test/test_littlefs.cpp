// esp32emu test — LittleFS
#include "LittleFS.h"
#include <cassert>
#include <cstdio>
#include <filesystem>

int main() {
    // Begin
    assert(LittleFS.begin());

    // Write a file
    auto f = LittleFS.open("/test.txt", "w");
    assert(f);
    f.print("Hello LittleFS");
    f.close();

    // File exists
    assert(LittleFS.exists("/test.txt"));

    // Read back
    auto r = LittleFS.open("/test.txt", "r");
    assert(r);
    auto content = r.readString();
    assert(content == "Hello LittleFS");
    r.close();

    // Rename
    assert(LittleFS.rename("/test.txt", "/renamed.txt"));
    assert(!LittleFS.exists("/test.txt"));
    assert(LittleFS.exists("/renamed.txt"));

    // Remove
    assert(LittleFS.remove("/renamed.txt"));
    assert(!LittleFS.exists("/renamed.txt"));

    // Subdirectory
    assert(LittleFS.mkdir("/subdir"));
    auto f2 = LittleFS.open("/subdir/data.txt", "w");
    f2.println("nested");
    f2.close();
    assert(LittleFS.exists("/subdir/data.txt"));

    LittleFS.end();
    printf("test_littlefs: all assertions passed\n");
    return 0;
}
