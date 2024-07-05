#ifndef _STATUS_LED_h
#define _STATUS_LED_h

#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>

class StatusLED {
    public:
        StatusLED(gpio_num_t _r, gpio_num_t _g, gpio_num_t _b);
        void booting();
        void active();
        void no_network();
        void sleeping();

    private:
        gpio_num_t r, g, b;
};

#endif