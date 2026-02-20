#pragma once
// esp32emu: ESP32 Camera driver mock for ESP32-CAM projects
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <functional>
#include "esp_err.h"

// Pixel formats
typedef enum {
    PIXFORMAT_RGB565,
    PIXFORMAT_YUV422,
    PIXFORMAT_GRAYSCALE,
    PIXFORMAT_JPEG,
    PIXFORMAT_RGB888,
    PIXFORMAT_RAW,
    PIXFORMAT_RGB444,
    PIXFORMAT_RGB555,
} pixformat_t;

// Frame sizes
typedef enum {
    FRAMESIZE_96X96,
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_HQVGA,    // 240x176
    FRAMESIZE_240X240,
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_CIF,      // 400x296
    FRAMESIZE_HVGA,     // 480x320
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_HD,       // 1280x720
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
    FRAMESIZE_INVALID,
} framesize_t;

// Camera configuration
typedef struct {
    int pin_pwdn;
    int pin_reset;
    int pin_xclk;
    int pin_sscb_sda;
    int pin_sscb_scl;
    int pin_d7;
    int pin_d6;
    int pin_d5;
    int pin_d4;
    int pin_d3;
    int pin_d2;
    int pin_d1;
    int pin_d0;
    int pin_vsync;
    int pin_href;
    int pin_pclk;
    int xclk_freq_hz;
    int ledc_timer;
    int ledc_channel;
    pixformat_t pixel_format;
    framesize_t frame_size;
    int jpeg_quality;   // 0-63, lower = higher quality
    size_t fb_count;
    int fb_location;    // CAMERA_FB_IN_PSRAM or CAMERA_FB_IN_DRAM
    int grab_mode;
} camera_config_t;

#define CAMERA_FB_IN_PSRAM  0
#define CAMERA_FB_IN_DRAM   1
#define CAMERA_GRAB_WHEN_EMPTY  0
#define CAMERA_GRAB_LATEST      1

// Camera frame buffer
typedef struct {
    uint8_t* buf;
    size_t len;
    size_t width;
    size_t height;
    pixformat_t format;
} camera_fb_t;

// Sensor settings
struct sensor_t {
    int8_t brightness;    // -2 to 2
    int8_t contrast;      // -2 to 2
    int8_t saturation;    // -2 to 2
    int8_t sharpness;     // -2 to 2
    uint8_t denoise;
    uint8_t special_effect; // 0-6
    uint8_t wb_mode;      // 0-4
    uint8_t awb;
    uint8_t awb_gain;
    uint8_t aec;
    uint8_t aec2;
    int8_t ae_level;      // -2 to 2
    uint16_t aec_value;
    uint8_t agc;
    uint8_t agc_gain;
    uint8_t gainceiling;
    uint8_t bpc;
    uint8_t wpc;
    uint8_t raw_gma;
    uint8_t lenc;
    uint8_t vflip;
    uint8_t hmirror;
    uint8_t dcw;
    uint8_t colorbar;
    framesize_t framesize;
    pixformat_t pixformat;

    // Function pointers for settings (mocked)
    int (*set_brightness)(void*, int) = nullptr;
    int (*set_contrast)(void*, int) = nullptr;
    int (*set_saturation)(void*, int) = nullptr;
    int (*set_framesize)(void*, framesize_t) = nullptr;
    int (*set_quality)(void*, int) = nullptr;
    int (*set_vflip)(void*, int) = nullptr;
    int (*set_hmirror)(void*, int) = nullptr;
};

namespace esp32emu {
namespace camera {

struct State {
    bool initialized = false;
    camera_config_t config{};
    sensor_t sensor{};
    std::vector<uint8_t> frame_data;
    camera_fb_t fb{};
    std::function<void(camera_fb_t*)> frame_generator;
};

inline State& state() { static State s; return s; }

// Test helper: inject frame data
inline void inject_frame(const uint8_t* data, size_t len, size_t w, size_t h, pixformat_t fmt) {
    auto& s = state();
    s.frame_data.assign(data, data + len);
    s.fb.buf = s.frame_data.data();
    s.fb.len = len;
    s.fb.width = w;
    s.fb.height = h;
    s.fb.format = fmt;
}

inline void inject_jpeg(size_t w = 640, size_t h = 480) {
    // Generate minimal JPEG-like stub (SOI + EOI markers)
    std::vector<uint8_t> jpeg = {0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10};
    jpeg.resize(128, 0x00); // pad
    jpeg.push_back(0xFF);
    jpeg.push_back(0xD9);
    inject_frame(jpeg.data(), jpeg.size(), w, h, PIXFORMAT_JPEG);
}

inline void reset() { state() = State{}; }

} // namespace camera
} // namespace esp32emu

// ---- Camera API ----

inline esp_err_t esp_camera_init(const camera_config_t* config) {
    auto& s = esp32emu::camera::state();
    if (config) s.config = *config;
    s.initialized = true;
    s.sensor.framesize = config ? config->frame_size : FRAMESIZE_VGA;
    s.sensor.pixformat = config ? config->pixel_format : PIXFORMAT_JPEG;
    // Generate a default frame if none injected
    if (s.frame_data.empty()) {
        esp32emu::camera::inject_jpeg(640, 480);
    }
    return ESP_OK;
}

inline esp_err_t esp_camera_deinit() {
    esp32emu::camera::reset();
    return ESP_OK;
}

inline camera_fb_t* esp_camera_fb_get() {
    auto& s = esp32emu::camera::state();
    if (!s.initialized) return nullptr;
    if (s.frame_generator) s.frame_generator(&s.fb);
    return &s.fb;
}

inline void esp_camera_fb_return(camera_fb_t* fb) {
    (void)fb; // no-op in mock
}

inline sensor_t* esp_camera_sensor_get() {
    auto& s = esp32emu::camera::state();
    if (!s.initialized) return nullptr;
    return &s.sensor;
}
