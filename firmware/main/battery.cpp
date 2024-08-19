#include "config.h"
#include "battery.h"

#include <esp_log.h>

static const char* TAG = "Battery";

Battery::Battery(adc1_channel_t _voltage) {
    voltage = _voltage;
    
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(voltage, ADC_ATTEN_DB_12);

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 1100, &adc_chars);
}

uint8_t Battery::measure() {
    uint32_t battery_level;
    esp_adc_cal_get_voltage((adc_channel_t)voltage, &adc_chars, &battery_level);

    uint8_t result = battery_level < CONFIG_BATTERY_CHECK_THRESH;
    
    ESP_LOGI(TAG, "%d mV (%s)", battery_level, result? "low" : "good");

    return result;
}
