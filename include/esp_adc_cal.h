#pragma once
// esp32emu — ESP-IDF ADC calibration mock

#include <cstdint>
#include <cstdio>

typedef enum {
    ADC_ATTEN_DB_0 = 0,
    ADC_ATTEN_DB_2_5 = 1,
    ADC_ATTEN_DB_6 = 2,
    ADC_ATTEN_DB_11 = 3,
    ADC_ATTEN_DB_12 = 3  // alias in newer IDF
} adc_atten_t;

typedef enum {
    ADC_WIDTH_BIT_9 = 0,
    ADC_WIDTH_BIT_10 = 1,
    ADC_WIDTH_BIT_11 = 2,
    ADC_WIDTH_BIT_12 = 3,
    ADC_WIDTH_MAX
} adc_bits_width_t;

typedef enum {
    ADC_UNIT_1 = 1,
    ADC_UNIT_2 = 2
} adc_unit_t;

typedef enum {
    ESP_ADC_CAL_VAL_EFUSE_VREF = 0,
    ESP_ADC_CAL_VAL_EFUSE_TP = 1,
    ESP_ADC_CAL_VAL_DEFAULT_VREF = 2,
    ESP_ADC_CAL_VAL_MAX
} esp_adc_cal_value_t;

typedef struct {
    adc_unit_t adc_num;
    adc_atten_t atten;
    adc_bits_width_t bit_width;
    uint32_t coeff_a;  // gain (mV per count * 65536)
    uint32_t coeff_b;  // offset (mV * 65536)
    uint32_t vref;
} esp_adc_cal_characteristics_t;

namespace esp32emu {
// Voltage ranges for each attenuation (approximate)
inline uint32_t adc_cal_max_mv(adc_atten_t atten) {
    switch (atten) {
        case ADC_ATTEN_DB_0:   return 1100;
        case ADC_ATTEN_DB_2_5: return 1500;
        case ADC_ATTEN_DB_6:   return 2200;
        case ADC_ATTEN_DB_11:  return 3300;
        default: return 3300;
    }
}
inline uint32_t adc_cal_max_raw(adc_bits_width_t w) {
    return (1u << (9 + (int)w)) - 1;
}
} // namespace esp32emu

inline esp_adc_cal_value_t esp_adc_cal_characterize(
    adc_unit_t unit, adc_atten_t atten, adc_bits_width_t width,
    uint32_t default_vref, esp_adc_cal_characteristics_t* chars)
{
    if (chars) {
        chars->adc_num = unit;
        chars->atten = atten;
        chars->bit_width = width;
        chars->vref = default_vref ? default_vref : 1100;
        uint32_t max_mv = esp32emu::adc_cal_max_mv(atten);
        uint32_t max_raw = esp32emu::adc_cal_max_raw(width);
        chars->coeff_a = (max_mv * 65536) / max_raw;
        chars->coeff_b = 0;
    }
    fprintf(stderr, "[esp32emu] ADC%d calibrated: atten=%d, width=%d\n", unit, atten, width);
    return ESP_ADC_CAL_VAL_DEFAULT_VREF;
}

inline uint32_t esp_adc_cal_raw_to_voltage(uint32_t raw,
    const esp_adc_cal_characteristics_t* chars)
{
    if (!chars) return 0;
    return (raw * chars->coeff_a + chars->coeff_b) / 65536;
}
