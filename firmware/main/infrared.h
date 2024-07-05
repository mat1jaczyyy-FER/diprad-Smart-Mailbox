#ifndef _INFRARED_h
#define _INFRARED_h

#include <freertos/FreeRTOS.h>
#include <driver/adc.h>
#include <driver/gpio.h>

#include "esp_adc_cal.h"

class Infrared {
    public:
        Infrared(gpio_num_t _en, adc1_channel_t _recv);
        uint8_t measure();

    private:
        gpio_num_t en;
        adc1_channel_t recv;
        esp_adc_cal_characteristics_t adc_chars;
};

#endif