#ifndef _INFRARED_h
#define _INFRARED_h

#include "config.h"

#if defined(CONFIG_USE_INFRARED)

#include <freertos/FreeRTOS.h>
#include <driver/adc.h>
#include <driver/gpio.h>

#include "esp_adc_cal.h"

class Infrared {
    public:
        Infrared(gpio_num_t _en_tx, gpio_num_t _en_rx, adc1_channel_t _rx);
        void set_config(uint32_t* config);
        uint8_t measure();

    private:
        gpio_num_t en_tx, en_rx;
        adc1_channel_t rx;
        esp_adc_cal_characteristics_t adc_chars;

        bool is_ge_lerp(uint32_t off, uint32_t on, float* thresh);
};

extern Infrared sensor;

#endif
#endif
