// esp32emu test — ADC calibration
#include "esp_adc_cal.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>

int main() {
    // Characterize ADC1 with 11dB attenuation, 12-bit
    esp_adc_cal_characteristics_t chars = {};
    auto val_type = esp_adc_cal_characterize(
        ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &chars);
    assert(val_type == ESP_ADC_CAL_VAL_DEFAULT_VREF);
    assert(chars.adc_num == ADC_UNIT_1);
    assert(chars.atten == ADC_ATTEN_DB_11);
    assert(chars.bit_width == ADC_WIDTH_BIT_12);
    assert(chars.vref == 1100);

    // Raw 0 → 0 mV
    assert(esp_adc_cal_raw_to_voltage(0, &chars) == 0);

    // Raw max (4095) → ~3300 mV
    uint32_t mv = esp_adc_cal_raw_to_voltage(4095, &chars);
    assert(mv >= 3290 && mv <= 3310);  // approximately 3300

    // Mid-range: raw 2048 → ~1650 mV
    uint32_t mid_mv = esp_adc_cal_raw_to_voltage(2048, &chars);
    assert(mid_mv >= 1640 && mid_mv <= 1660);

    // Different attenuation: 0dB → max ~1100mV
    esp_adc_cal_characteristics_t chars0 = {};
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12, 1100, &chars0);
    uint32_t mv0 = esp_adc_cal_raw_to_voltage(4095, &chars0);
    assert(mv0 >= 1090 && mv0 <= 1110);

    // Different width: 10-bit (max 1023)
    esp_adc_cal_characteristics_t chars10 = {};
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_10, 0, &chars10);
    assert(chars10.vref == 1100);  // default vref
    uint32_t mv10 = esp_adc_cal_raw_to_voltage(1023, &chars10);
    assert(mv10 >= 3290 && mv10 <= 3310);

    // Null chars
    assert(esp_adc_cal_raw_to_voltage(100, nullptr) == 0);

    printf("test_adc_cal: all assertions passed\n");
    return 0;
}
