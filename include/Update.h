#pragma once
// esp32emu: Arduino Update library mock for OTA firmware updates
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <functional>
#include <vector>
#include <string>
#include "Arduino.h"

#define UPDATE_ERROR_OK          0
#define UPDATE_ERROR_WRITE       1
#define UPDATE_ERROR_ERASE       2
#define UPDATE_ERROR_READ        3
#define UPDATE_ERROR_SPACE       4
#define UPDATE_ERROR_SIZE        5
#define UPDATE_ERROR_STREAM      6
#define UPDATE_ERROR_MD5         7
#define UPDATE_ERROR_MAGIC_BYTE  8
#define UPDATE_ERROR_ACTIVATE    9
#define UPDATE_ERROR_NO_PARTITION 10
#define UPDATE_ERROR_BAD_ARGUMENT 11
#define UPDATE_ERROR_ABORT       12

#define U_FLASH   0
#define U_SPIFFS  100

#define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF

class UpdateClass {
public:
    using THandlerFunction_Progress = std::function<void(size_t, size_t)>;

    bool begin(size_t size = UPDATE_SIZE_UNKNOWN, int command = U_FLASH, int ledPin = -1, uint8_t ledOn = 0) {
        (void)ledPin; (void)ledOn;
        _command = command;
        _size = size;
        _written = 0;
        _buffer.clear();
        _error = UPDATE_ERROR_OK;
        _isRunning = true;
        _finished = false;
        _md5Expected.clear();
        return true;
    }

    size_t write(const uint8_t* data, size_t len) {
        if (!_isRunning) { _error = UPDATE_ERROR_WRITE; return 0; }
        _buffer.insert(_buffer.end(), data, data + len);
        _written += len;
        if (_onProgress) _onProgress(_written, _size);
        return len;
    }

    size_t writeStream(void* /*stream*/) {
        // Mock: simulate writing 1024 bytes from a stream
        _written += 1024;
        if (_onProgress) _onProgress(_written, _size);
        return 1024;
    }

    bool end(bool evenIfRemaining = false) {
        if (!_isRunning) return false;
        if (!evenIfRemaining && _size != UPDATE_SIZE_UNKNOWN && _written != _size) {
            _error = UPDATE_ERROR_SIZE;
            _isRunning = false;
            return false;
        }
        if (!_md5Expected.empty() && _md5Expected != md5String().c_str()) {
            _error = UPDATE_ERROR_MD5;
            _isRunning = false;
            return false;
        }
        _isRunning = false;
        _finished = true;
        return true;
    }

    void abort() {
        _error = UPDATE_ERROR_ABORT;
        _isRunning = false;
        _finished = false;
        _buffer.clear();
    }

    bool isRunning() const { return _isRunning; }
    bool isFinished() const { return _finished; }
    bool hasError() const { return _error != UPDATE_ERROR_OK; }
    uint8_t getError() const { return _error; }
    const char* getErrorString() const {
        switch (_error) {
            case UPDATE_ERROR_OK:           return "No Error";
            case UPDATE_ERROR_WRITE:        return "Write Failed";
            case UPDATE_ERROR_ERASE:        return "Erase Failed";
            case UPDATE_ERROR_SIZE:         return "Size Mismatch";
            case UPDATE_ERROR_MD5:          return "MD5 Mismatch";
            case UPDATE_ERROR_ABORT:        return "Aborted";
            default:                        return "Unknown Error";
        }
    }
    void clearError() { _error = UPDATE_ERROR_OK; }

    size_t size() const { return _size; }
    size_t progress() const { return _written; }
    size_t remaining() const { return (_size == UPDATE_SIZE_UNKNOWN) ? 0 : (_size > _written ? _size - _written : 0); }
    int command() const { return _command; }

    void setMD5(const char* md5) { _md5Expected = md5 ? md5 : ""; }
    String md5String() const { return "d41d8cd98f00b204e9800998ecf8427e"; } // mock empty MD5

    bool canRollBack() const { return _finished; }
    bool rollBack() { _finished = false; return true; }

    void onProgress(THandlerFunction_Progress fn) { _onProgress = fn; }

    // Test helpers
    const std::vector<uint8_t>& getBuffer() const { return _buffer; }
    void reset() { _buffer.clear(); _written = 0; _size = 0; _error = UPDATE_ERROR_OK; _isRunning = false; _finished = false; }

private:
    size_t _size = 0;
    size_t _written = 0;
    int _command = U_FLASH;
    uint8_t _error = UPDATE_ERROR_OK;
    bool _isRunning = false;
    bool _finished = false;
    std::string _md5Expected;
    std::vector<uint8_t> _buffer;
    THandlerFunction_Progress _onProgress;
};

extern UpdateClass Update;
