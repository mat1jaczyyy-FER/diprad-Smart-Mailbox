#include "config.h"
#include "status_led.h"

#include <esp_log.h>

static const char* TAG = "StatusLED";

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

void StatusLED::booting() {
    ESP_LOGI(TAG, "Booting");
    gpio_set_level(r, 1);
    gpio_set_level(g, 0);
    gpio_set_level(b, 0);
}

void StatusLED::active() {
    ESP_LOGI(TAG, "Active");
    gpio_set_level(r, 0);
    gpio_set_level(g, 1);
    gpio_set_level(b, 0);
}

void StatusLED::no_network() {
    ESP_LOGI(TAG, "No Network");
    gpio_set_level(r, 0);
    gpio_set_level(g, 0);
    gpio_set_level(b, 1);
}

void StatusLED::sleeping() {
    ESP_LOGI(TAG, "Sleeping");
    gpio_set_level(r, 0);
    gpio_set_level(g, 0);
    gpio_set_level(b, 0);
}
