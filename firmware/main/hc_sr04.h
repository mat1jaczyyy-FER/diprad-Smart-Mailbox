#ifndef _HC_SR04_h
#define _HC_SR04_h

#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>

class HC_SR04 {
    public:
        HC_SR04(gpio_num_t _trig, gpio_num_t _echo);
        uint8_t measure();

    private:
        gpio_num_t trig, echo;
        float roundtrip();

        uint32_t empty_from, empty_to;
        void set_config(uint32_t* config);
        bool is_out_of_range(float roundtrip);
};

#endif