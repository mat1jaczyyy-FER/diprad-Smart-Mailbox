#include "config.h"
#include "status_led.h"

#include <esp_log.h>

__attribute__((unused))
static const char* TAG = "StatusLED";

#if defined(CONFIG_USE_STATUS_LED)
    StatusLED::StatusLED(gpio_num_t _r, gpio_num_t _g, gpio_num_t _b) {
        r = _r;
        g = _g;
        b = _b;

        gpio_set_direction(r, GPIO_MODE_OUTPUT);
        gpio_set_direction(g, GPIO_MODE_OUTPUT);
        gpio_set_direction(b, GPIO_MODE_OUTPUT);

        gpio_set_level(r, 0);
        gpio_set_level(g, 0);
        gpio_set_level(b, 0);
    }
#else
    StatusLED::StatusLED() {}
#endif

void StatusLED::set(uint32_t rv, uint32_t gv, uint32_t bv) {
    #if defined(CONFIG_USE_STATUS_LED)
        gpio_set_level(r, rv);
        gpio_set_level(g, gv);
        gpio_set_level(b, bv);
    #endif
}

void StatusLED::active() {
    //ESP_LOGI(TAG, "Active");
    this->set(0, 1, 0);
}

void StatusLED::no_wifi() {
    //ESP_LOGI(TAG, "No WiFi Available");
    this->set(0, 0, 1);
}

void StatusLED::wifi_retrying() {
    //ESP_LOGI(TAG, "WiFi Retrying");
    this->set(1, 0, 0);
}

void StatusLED::sleeping() {
    //ESP_LOGI(TAG, "Sleeping");
    this->set(0, 0, 0);
}

#if defined(CONFIG_USE_STATUS_LED)
    StatusLED status_led(CONFIG_GPIO_STATUS_LED_R, CONFIG_GPIO_STATUS_LED_G, CONFIG_GPIO_STATUS_LED_B);
#else
    StatusLED status_led;
#endif
