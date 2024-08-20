#ifndef _HC_SR04_h
#define _HC_SR04_h

#include "config.h"

#if defined(CONFIG_USE_ULTRASONIC)

#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>

class HC_SR04 {
    public:
        HC_SR04(gpio_num_t _en, gpio_num_t _trig, gpio_num_t _echo);
        void set_config(uint32_t* config);
        uint8_t measure();

    private:
        gpio_num_t en, trig, echo;
        float roundtrip();
        bool is_out_of_range(float roundtrip);
};

extern HC_SR04 sensor;

#endif
#endif
