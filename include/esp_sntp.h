#pragma once
// esp32emu — SNTP time synchronization mock
// Simulates esp_sntp / sntp APIs. Time is backed by host system clock.

#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstring>
#include <string>
#include <sys/time.h>
#include <functional>

typedef enum {
    SNTP_SYNC_MODE_IMMED = 0,
    SNTP_SYNC_MODE_SMOOTH
} sntp_sync_mode_t;

typedef enum {
    SNTP_SYNC_STATUS_RESET = 0,
    SNTP_SYNC_STATUS_COMPLETED,
    SNTP_SYNC_STATUS_IN_PROGRESS
} sntp_sync_status_t;

typedef enum {
    SNTP_OPMODE_POLL = 0,
    SNTP_OPMODE_LISTENONLY
} sntp_operatingmode_t;

namespace esp32emu {
struct SntpState {
    bool initialized = false;
    sntp_operatingmode_t opmode = SNTP_OPMODE_POLL;
    sntp_sync_mode_t sync_mode = SNTP_SYNC_MODE_IMMED;
    sntp_sync_status_t sync_status = SNTP_SYNC_STATUS_RESET;
    std::string servers[4];
    int server_count = 0;
    std::function<void(struct timeval*)> sync_cb;
    // Test helper: offset seconds to add to system time
    int64_t time_offset_sec = 0;
    void reset() { *this = SntpState{}; }
};
inline SntpState& sntp_state() { static SntpState s; return s; }
} // namespace esp32emu

inline void sntp_setoperatingmode(sntp_operatingmode_t mode) {
    esp32emu::sntp_state().opmode = mode;
}

inline void sntp_setservername(int idx, const char* server) {
    if (idx >= 0 && idx < 4) {
        esp32emu::sntp_state().servers[idx] = server;
        if (idx >= esp32emu::sntp_state().server_count)
            esp32emu::sntp_state().server_count = idx + 1;
    }
}

inline const char* sntp_getservername(int idx) {
    if (idx >= 0 && idx < 4) return esp32emu::sntp_state().servers[idx].c_str();
    return "";
}

inline void sntp_init() {
    auto& s = esp32emu::sntp_state();
    s.initialized = true;
    s.sync_status = SNTP_SYNC_STATUS_COMPLETED;  // instant sync in emulator
    fprintf(stderr, "[esp32emu] SNTP initialized (using host clock)\n");
    if (s.sync_cb) {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        s.sync_cb(&tv);
    }
}

inline void sntp_stop() {
    esp32emu::sntp_state().initialized = false;
    esp32emu::sntp_state().sync_status = SNTP_SYNC_STATUS_RESET;
}

inline bool sntp_enabled() { return esp32emu::sntp_state().initialized; }

inline sntp_sync_status_t sntp_get_sync_status() {
    return esp32emu::sntp_state().sync_status;
}

inline void sntp_set_sync_mode(sntp_sync_mode_t mode) {
    esp32emu::sntp_state().sync_mode = mode;
}

inline sntp_sync_mode_t sntp_get_sync_mode() {
    return esp32emu::sntp_state().sync_mode;
}

// esp_sntp aliases (newer ESP-IDF API)
inline void esp_sntp_setoperatingmode(sntp_operatingmode_t m) { sntp_setoperatingmode(m); }
inline void esp_sntp_setservername(int i, const char* s) { sntp_setservername(i, s); }
inline void esp_sntp_init() { sntp_init(); }
inline void esp_sntp_stop() { sntp_stop(); }
inline bool esp_sntp_enabled() { return sntp_enabled(); }
inline sntp_sync_status_t esp_sntp_get_sync_status() { return sntp_get_sync_status(); }

// Test helper: set time callback
inline void esp32emu_sntp_set_sync_callback(std::function<void(struct timeval*)> cb) {
    esp32emu::sntp_state().sync_cb = cb;
}

// Test helper: inject time offset
inline void esp32emu_sntp_set_time_offset(int64_t offset_sec) {
    esp32emu::sntp_state().time_offset_sec = offset_sec;
}
