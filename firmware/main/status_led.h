#ifndef _STATUS_LED_h
#define _STATUS_LED_h

#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>

class StatusLED {
    public:
        #if defined(CONFIG_USE_STATUS_LED)
            StatusLED(gpio_num_t _r, gpio_num_t _g, gpio_num_t _b);
        #else
            StatusLED();
        #endif

        void active();
        void dpp_authentication();
        void wifi_retrying();
        void sleeping();

    #if defined(CONFIG_USE_STATUS_LED)
        private:
            void set(uint32_t rv, uint32_t gv, uint32_t bv);
            gpio_num_t r, g, b;
    #endif
};

extern StatusLED status_led;

#endif