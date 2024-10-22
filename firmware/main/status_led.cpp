#include "config.h"
#include "status_led.h"

#include <esp_log.h>

__attribute__((unused))
static const char* TAG = "StatusLED";

#if defined(CONFIG_USE_STATUS_LED)
    StatusLED::StatusLED(rmt_channel_t _rmt, gpio_num_t _gpio) {
        rmt = _rmt;
        gpio = _gpio;
    }
#else
    StatusLED::StatusLED() {}
#endif

void StatusLED::init() {
    #if defined(CONFIG_USE_STATUS_LED)
        led = led_strip_init(rmt, gpio, 1);
    #endif
}

void StatusLED::set(uint8_t rv, uint8_t gv, uint8_t bv) {
    #if defined(CONFIG_USE_STATUS_LED)
        led->set_pixel(led, 0, rv, gv, bv);
        led->refresh(led, 50);
    #endif
}

void StatusLED::active() {
    //ESP_LOGI(TAG, "Active");
    this->set(0, 255, 0);
}

void StatusLED::dpp_authentication() {
    //ESP_LOGI(TAG, "DPP Authentication");
    this->set(0, 0, 255);
}

void StatusLED::wifi_retrying() {
    //ESP_LOGI(TAG, "WiFi Retrying");
    this->set(255, 0, 0);
}

void StatusLED::api_retrying() {
    //ESP_LOGI(TAG, "API Retrying");
    this->set(255, 30, 0);
}

void StatusLED::sleeping() {
    //ESP_LOGI(TAG, "Sleeping");
    this->set(0, 0, 0);
}

#if defined(CONFIG_USE_STATUS_LED)
    StatusLED status_led(CONFIG_STATUS_LED_RMT, CONFIG_STATUS_LED_GPIO);
#else
    StatusLED status_led;
#endif
