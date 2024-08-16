#include "config.h"
#include "infrared.h"

#include <esp_log.h>

static const char* TAG = "Infrared";

#if !defined(CONFIG_MAIL_EMPTY_FROM) || !defined(CONFIG_MAIL_EMPTY_TO)
    #error "CONFIG_MAIL_EMPTY_FROM and CONFIG_MAIL_EMPTY_TO must be defined"
#endif

Infrared::Infrared(gpio_num_t _en_tx, gpio_num_t _en_rx, adc1_channel_t _rx) {
    en_tx = _en_tx;
    en_rx = _en_rx;
    rx = _rx;

    gpio_set_direction(en_tx, GPIO_MODE_OUTPUT);
    gpio_set_direction(en_rx, GPIO_MODE_OUTPUT);

    gpio_set_level(en_tx, 0);
    gpio_set_level(en_rx, 0);
    
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(rx, ADC_ATTEN_DB_12);

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 1100, &adc_chars);
}

uint8_t Infrared::measure() {
    gpio_set_level(en_tx, 0);
    gpio_set_level(en_rx, 1);

    uint32_t baseline;
    esp_adc_cal_get_voltage((adc_channel_t)rx, &adc_chars, &baseline);

    gpio_set_level(en_tx, 1);
    
    uint32_t result;
    esp_adc_cal_get_voltage((adc_channel_t)rx, &adc_chars, &result);

    gpio_set_level(en_tx, 0);
    gpio_set_level(en_rx, 0);

    ESP_LOGI(TAG, "off => %u mV, on => %u mV", baseline, result);

    return !(CONFIG_MAIL_EMPTY_FROM <= result && result <= CONFIG_MAIL_EMPTY_TO);
}
