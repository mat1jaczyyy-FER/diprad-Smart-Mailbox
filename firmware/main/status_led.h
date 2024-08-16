#ifndef _STATUS_LED_h
#define _STATUS_LED_h

#include <freertos/FreeRTOS.h>
#include <driver/gpio.h>
#include <hal/rmt_types.h>

#include <led_strip.h>

class StatusLED {
    public:
        #if defined(CONFIG_USE_STATUS_LED)
            StatusLED(rmt_channel_t rmt, gpio_num_t gpio);
        #else
            StatusLED();
        #endif
        
        void init();
        
        void active();
        void dpp_authentication();
        void wifi_retrying();
        void sleeping();

    #if defined(CONFIG_USE_STATUS_LED)
        private:
            void set(uint8_t rv, uint8_t gv, uint8_t bv);
            rmt_channel_t rmt;
            gpio_num_t gpio;
            led_strip_t* led;
    #endif
};

extern StatusLED status_led;

#endif