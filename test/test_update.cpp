#include "Update.h"
#include <cassert>
#include <cstdio>
#include <vector>

int main() {
    // Test basic update lifecycle
    {
        Update.reset();
        assert(Update.begin(1024, U_FLASH));
        assert(Update.isRunning());
        assert(!Update.isFinished());
        assert(Update.size() == 1024);
        assert(Update.progress() == 0);
        assert(Update.remaining() == 1024);
        assert(Update.command() == U_FLASH);
    }

    // Test write and progress
    {
        Update.reset();
        assert(Update.begin(256));
        uint8_t buf[128];
        memset(buf, 0xAA, 128);
        assert(Update.write(buf, 128) == 128);
        assert(Update.progress() == 128);
        assert(Update.remaining() == 128);
        assert(Update.write(buf, 128) == 128);
        assert(Update.progress() == 256);
        assert(Update.remaining() == 0);
        assert(Update.end());
        assert(Update.isFinished());
        assert(!Update.hasError());
    }

    // Test size mismatch error
    {
        Update.reset();
        assert(Update.begin(256));
        uint8_t buf[64];
        Update.write(buf, 64);
        assert(!Update.end()); // size mismatch
        assert(Update.hasError());
        assert(Update.getError() == UPDATE_ERROR_SIZE);
    }

    // Test end with evenIfRemaining
    {
        Update.reset();
        assert(Update.begin(256));
        uint8_t buf[64];
        Update.write(buf, 64);
        assert(Update.end(true)); // force finish
        assert(Update.isFinished());
    }

    // Test unknown size
    {
        Update.reset();
        assert(Update.begin()); // UPDATE_SIZE_UNKNOWN
        uint8_t buf[100];
        Update.write(buf, 100);
        assert(Update.end());
        assert(Update.isFinished());
    }

    // Test abort
    {
        Update.reset();
        assert(Update.begin(1024));
        uint8_t buf[10];
        Update.write(buf, 10);
        Update.abort();
        assert(!Update.isRunning());
        assert(!Update.isFinished());
        assert(Update.hasError());
        assert(Update.getError() == UPDATE_ERROR_ABORT);
    }

    // Test progress callback
    {
        Update.reset();
        size_t lastCurrent = 0, lastTotal = 0;
        Update.onProgress([&](size_t current, size_t total) {
            lastCurrent = current;
            lastTotal = total;
        });
        assert(Update.begin(512));
        uint8_t buf[256];
        Update.write(buf, 256);
        assert(lastCurrent == 256);
        assert(lastTotal == 512);
    }

    // Test SPIFFS update type
    {
        Update.reset();
        assert(Update.begin(100, U_SPIFFS));
        assert(Update.command() == U_SPIFFS);
    }

    // Test error string
    {
        Update.reset();
        assert(Update.begin(10));
        Update.abort();
        const char* err = Update.getErrorString();
        assert(err != nullptr);
        assert(strcmp(err, "Aborted") == 0);
    }

    // Test buffer content
    {
        Update.reset();
        assert(Update.begin(4));
        uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
        Update.write(data, 4);
        const auto& buf = Update.getBuffer();
        assert(buf.size() == 4);
        assert(buf[0] == 0xDE && buf[3] == 0xEF);
    }

    // Test rollback
    {
        Update.reset();
        assert(Update.begin(0));
        assert(Update.end(true));
        assert(Update.canRollBack());
        assert(Update.rollBack());
        assert(!Update.isFinished());
    }

    printf("test_update: all assertions passed\n");
    return 0;
}
