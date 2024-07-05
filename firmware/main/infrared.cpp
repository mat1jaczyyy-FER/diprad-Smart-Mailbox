#include "config.h"
#include "infrared.h"

#include <esp_log.h>

static const char* TAG = "Infrared";

#if defined(CONFIG_USE_INFRARED) && !defined(CONFIG_MAIL_INFRARED_EMPTY_FROM) && !defined(CONFIG_MAIL_INFRARED_EMPTY_TO)
    #error "At least one of CONFIG_MAIL_INFRARED_EMPTY_FROM or CONFIG_MAIL_INFRARED_EMPTY_TO must be defined"
#endif

Infrared::Infrared(gpio_num_t _en, adc1_channel_t _recv) {
    en = _en;
    recv = _recv;

    gpio_set_direction(en, GPIO_MODE_OUTPUT);
    gpio_set_level(en, 0);
    
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(recv, ADC_ATTEN_DB_12);

    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 1100, &adc_chars);
}

uint8_t Infrared::measure() {
    gpio_set_level(en, 1);
    
    uint32_t result;
    esp_adc_cal_get_voltage((adc_channel_t)recv, &adc_chars, &result);

    gpio_set_level(en, 0);

    ESP_LOGI(TAG, "%u mV", result);

    return !(
        #if defined(CONFIG_MAIL_INFRARED_EMPTY_FROM)
            CONFIG_MAIL_INFRARED_EMPTY_FROM <= result

            #if defined(CONFIG_MAIL_INFRARED_EMPTY_TO)
                &&
            #endif
        #endif

        #if defined(CONFIG_MAIL_INFRARED_EMPTY_TO)
            result <= CONFIG_MAIL_INFRARED_EMPTY_TO
        #endif
    );
}
