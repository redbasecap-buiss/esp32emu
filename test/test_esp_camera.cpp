#include "esp_camera.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test init/deinit
    {
        esp32emu::camera::reset();
        camera_config_t config{};
        config.pixel_format = PIXFORMAT_JPEG;
        config.frame_size = FRAMESIZE_VGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
        assert(esp_camera_init(&config) == ESP_OK);
        auto& s = esp32emu::camera::state();
        assert(s.initialized);
        assert(s.config.jpeg_quality == 12);
        assert(esp_camera_deinit() == ESP_OK);
        assert(!esp32emu::camera::state().initialized);
    }

    // Test frame capture with default frame
    {
        esp32emu::camera::reset();
        camera_config_t config{};
        config.pixel_format = PIXFORMAT_JPEG;
        config.frame_size = FRAMESIZE_VGA;
        config.fb_count = 1;
        esp_camera_init(&config);

        camera_fb_t* fb = esp_camera_fb_get();
        assert(fb != nullptr);
        assert(fb->len > 0);
        assert(fb->width == 640);
        assert(fb->height == 480);
        assert(fb->format == PIXFORMAT_JPEG);
        // Check JPEG markers
        assert(fb->buf[0] == 0xFF);
        assert(fb->buf[1] == 0xD8);
        esp_camera_fb_return(fb);
        esp_camera_deinit();
    }

    // Test custom frame injection
    {
        esp32emu::camera::reset();
        camera_config_t config{};
        config.pixel_format = PIXFORMAT_GRAYSCALE;
        config.frame_size = FRAMESIZE_QVGA;
        config.fb_count = 1;
        esp_camera_init(&config);

        uint8_t custom_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
        esp32emu::camera::inject_frame(custom_data, 5, 320, 240, PIXFORMAT_GRAYSCALE);

        camera_fb_t* fb = esp_camera_fb_get();
        assert(fb != nullptr);
        assert(fb->len == 5);
        assert(fb->width == 320);
        assert(fb->height == 240);
        assert(fb->format == PIXFORMAT_GRAYSCALE);
        assert(fb->buf[0] == 0x01);
        esp_camera_deinit();
    }

    // Test sensor settings
    {
        esp32emu::camera::reset();
        camera_config_t config{};
        config.pixel_format = PIXFORMAT_JPEG;
        config.frame_size = FRAMESIZE_SVGA;
        config.fb_count = 1;
        esp_camera_init(&config);

        sensor_t* sensor = esp_camera_sensor_get();
        assert(sensor != nullptr);
        assert(sensor->framesize == FRAMESIZE_SVGA);
        assert(sensor->pixformat == PIXFORMAT_JPEG);

        sensor->brightness = 1;
        sensor->vflip = 1;
        sensor->hmirror = 1;
        assert(sensor->brightness == 1);
        assert(sensor->vflip == 1);
        esp_camera_deinit();
    }

    // Test fb_get without init returns nullptr
    {
        esp32emu::camera::reset();
        assert(esp_camera_fb_get() == nullptr);
        assert(esp_camera_sensor_get() == nullptr);
    }

    // Test frame generator callback
    {
        esp32emu::camera::reset();
        camera_config_t config{};
        config.pixel_format = PIXFORMAT_JPEG;
        config.frame_size = FRAMESIZE_VGA;
        config.fb_count = 1;
        esp_camera_init(&config);

        int call_count = 0;
        esp32emu::camera::state().frame_generator = [&](camera_fb_t* fb) {
            call_count++;
            (void)fb;
        };

        esp_camera_fb_get();
        assert(call_count == 1);
        esp_camera_fb_get();
        assert(call_count == 2);
        esp_camera_deinit();
    }

    printf("test_esp_camera: all assertions passed\n");
    return 0;
}
