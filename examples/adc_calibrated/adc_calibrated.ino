// ADC calibration example — convert raw readings to millivolts
#include <Arduino.h>
#include "esp_adc_cal.h"

#define ADC_PIN  34
#define DEFAULT_VREF  1100

esp_adc_cal_characteristics_t adc_chars;

void setup() {
    Serial.begin(115200);
    Serial.println("ADC Calibration Example");

    // Characterize ADC
    auto val_type = esp_adc_cal_characterize(
        ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);

    Serial.print("Calibration type: ");
    switch (val_type) {
        case ESP_ADC_CAL_VAL_EFUSE_VREF: Serial.println("eFuse Vref"); break;
        case ESP_ADC_CAL_VAL_EFUSE_TP:   Serial.println("eFuse Two Point"); break;
        default: Serial.println("Default Vref"); break;
    }
}

void loop() {
    int raw = analogRead(ADC_PIN);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(raw, &adc_chars);

    Serial.print("Raw: ");
    Serial.print(raw);
    Serial.print("  Voltage: ");
    Serial.print(voltage);
    Serial.println(" mV");

    delay(1000);
}
