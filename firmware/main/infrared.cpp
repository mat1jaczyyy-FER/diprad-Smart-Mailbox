#include "config.h"
#include "infrared.h"

#include <string.h>

#include <esp_log.h>

#include "sleep.h"

static const char* TAG = "Infrared";

Infrared::Infrared(gpio_num_t _en_tx, gpio_num_t _en_rx, adc1_channel_t _rx) {
    lerp_n = 4;

    lerp[0][0] = 0;
    lerp[0][1] = 58;

    lerp[1][0] = 30;
    lerp[1][1] = 58;

    lerp[2][0] = 75;
    lerp[2][1] = 112;

    lerp[3][0] = 200;
    lerp[3][1] = 237;

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

void Infrared::set_config(uint32_t* config) {
    lerp_n = *(config++);
    memset(lerp, 0, sizeof(lerp));
    memcpy(lerp, config, sizeof(lerp[0]) * lerp_n);
}

bool Infrared::is_ge_lerp(uint32_t off, uint32_t on, float* thresh) {
    for (int i = 0; i < lerp_n; i++) {
        bool inf = i == lerp_n - 1;

        uint32_t x0 = lerp[i-inf][0];
        uint32_t y0 = lerp[i-inf][1];
        uint32_t x1 = lerp[i-inf+1][0];
        uint32_t y1 = lerp[i-inf+1][1];

        if (inf || (off >= x0 && off <= x1)) {
            float a = (float)(y1 - y0) / (x1 - x0);
            *thresh = y0 + a * (off - x0);

            return on >= *thresh;
        }
    }

    return false;
}

uint8_t Infrared::measure() {
    uint8_t success = 0;

    gpio_set_level(en_tx, 0);

    for (uint8_t i = 0; i < CONFIG_MEASURE_ITERATIONS - CONFIG_MEASURE_SUCCESS + 1 + success; i++) {
        gpio_set_level(en_rx, 1);

        uint32_t baseline;
        esp_adc_cal_get_voltage((adc_channel_t)rx, &adc_chars, &baseline);

        gpio_set_level(en_tx, 1);
        
        uint32_t result;
        esp_adc_cal_get_voltage((adc_channel_t)rx, &adc_chars, &result);

        gpio_set_level(en_tx, 0);
        gpio_set_level(en_rx, 0);


        if (i >= CONFIG_MEASURE_DISCARD) {
            float thresh = 0;
            bool is_ge = this->is_ge_lerp(baseline, result, &thresh);

            ESP_LOGI(TAG, "off => %d mV, thresh => %.1f mV, on => %d mV", baseline, thresh, result);

            if (is_ge) {
                if (++success >= CONFIG_MEASURE_SUCCESS) {
                    break;
                }
            } else {
                success = 0;
            }
        } else { 
            ESP_LOGI(TAG, "off => %d mV, on => %d mV, discarded", baseline, result);
        }

        light_sleep(CONFIG_MEASURE_DELAY);
    }

    return success;
}
