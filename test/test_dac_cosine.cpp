// esp32emu test — DAC cosine wave generator
#include "driver/dac_cosine.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static void test_new_channel_and_start() {
    esp32emu_dac_cosine_test::reset();
    dac_cosine_config_t cfg = {};
    cfg.chan_id = 0;
    cfg.freq_hz = 1000;
    cfg.atten = DAC_COSINE_ATTEN_DB_0;
    cfg.phase = DAC_COSINE_PHASE_0;
    cfg.offset = 0;

    dac_cosine_handle_t handle = nullptr;
    assert(dac_cosine_new_channel(&cfg, &handle) == ESP_OK);
    assert(handle != nullptr);
    assert(!esp32emu_dac_cosine_test::isStarted());

    assert(dac_cosine_start(handle) == ESP_OK);
    assert(esp32emu_dac_cosine_test::isStarted());
    assert(esp32emu_dac_cosine_test::getFrequency() == 1000);
    assert(esp32emu_dac_cosine_test::getChannel() == 0);
}

static void test_stop_and_delete() {
    esp32emu_dac_cosine_test::reset();
    dac_cosine_config_t cfg = {};
    cfg.chan_id = 1;
    cfg.freq_hz = 500;
    dac_cosine_handle_t handle = nullptr;
    dac_cosine_new_channel(&cfg, &handle);
    dac_cosine_start(handle);
    assert(esp32emu_dac_cosine_test::isStarted());

    assert(dac_cosine_stop(handle) == ESP_OK);
    assert(!esp32emu_dac_cosine_test::isStarted());

    assert(dac_cosine_del_channel(handle) == ESP_OK);
}

static void test_attenuation() {
    esp32emu_dac_cosine_test::reset();
    dac_cosine_config_t cfg = {};
    cfg.freq_hz = 100;
    cfg.atten = DAC_COSINE_ATTEN_DB_6;
    dac_cosine_handle_t handle = nullptr;
    dac_cosine_new_channel(&cfg, &handle);
    assert(esp32emu_dac_cosine_test::getAttenuation() == DAC_COSINE_ATTEN_DB_6);
}

static void test_voltage_calculation() {
    esp32emu_dac_cosine_test::reset();
    dac_cosine_config_t cfg = {};
    cfg.freq_hz = 1;  // 1 Hz for easy math
    cfg.atten = DAC_COSINE_ATTEN_DB_0;
    cfg.phase = DAC_COSINE_PHASE_0;
    cfg.offset = 0;
    dac_cosine_handle_t handle = nullptr;
    dac_cosine_new_channel(&cfg, &handle);
    dac_cosine_start(handle);

    // At t=0, cos(0)=1, so voltage = 127*1 + 128 = 255
    double v0 = esp32emu_dac_cosine_test::voltageAt(0.0);
    assert(fabs(v0 - 255.0) < 0.1);

    // At t=0.5 (half period), cos(pi)=-1, voltage = 127*(-1) + 128 = 1
    double v05 = esp32emu_dac_cosine_test::voltageAt(0.5);
    assert(fabs(v05 - 1.0) < 0.1);
}

static void test_phase_180() {
    esp32emu_dac_cosine_test::reset();
    dac_cosine_config_t cfg = {};
    cfg.freq_hz = 1;
    cfg.atten = DAC_COSINE_ATTEN_DB_0;
    cfg.phase = DAC_COSINE_PHASE_180;
    cfg.offset = 0;
    dac_cosine_handle_t handle = nullptr;
    dac_cosine_new_channel(&cfg, &handle);
    dac_cosine_start(handle);

    // At t=0 with phase 180, cos(pi)=-1, voltage = 127*(-1) + 128 = 1
    double v0 = esp32emu_dac_cosine_test::voltageAt(0.0);
    assert(fabs(v0 - 1.0) < 0.1);
}

static void test_null_args() {
    dac_cosine_handle_t handle = nullptr;
    assert(dac_cosine_new_channel(nullptr, &handle) == ESP_ERR_INVALID_ARG);
    assert(dac_cosine_start(nullptr) == ESP_ERR_INVALID_ARG);
    assert(dac_cosine_stop(nullptr) == ESP_ERR_INVALID_ARG);
    assert(dac_cosine_del_channel(nullptr) == ESP_ERR_INVALID_ARG);
}

int main() {
    test_new_channel_and_start();
    test_stop_and_delete();
    test_attenuation();
    test_voltage_calculation();
    test_phase_180();
    test_null_args();
    printf("test_dac_cosine: all assertions passed\n");
    return 0;
}
