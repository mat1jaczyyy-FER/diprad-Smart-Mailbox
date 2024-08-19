#ifndef _BATTERY_h
#define _BATTERY_h

#include <freertos/FreeRTOS.h>
#include <driver/adc.h>

#include "esp_adc_cal.h"

class Battery {
    public:
        Battery(adc1_channel_t _voltage);
        uint8_t measure();

    private:
        adc1_channel_t voltage;
        esp_adc_cal_characteristics_t adc_chars;
};

#endif