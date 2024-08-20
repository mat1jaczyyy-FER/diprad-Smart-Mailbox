#include "config.h"
#if defined(CONFIG_USE_ULTRASONIC)

#include "hc_sr04.h"

#include <esp_log.h>

#include "sleep.h"

static const char* TAG = "HC_SR04";

RTC_DATA_ATTR uint32_t empty_from, empty_to;

HC_SR04::HC_SR04(gpio_num_t _en, gpio_num_t _trig, gpio_num_t _echo) {
    en = _en;
    trig = _trig;
    echo = _echo;

    gpio_set_direction(en, GPIO_MODE_OUTPUT);
    gpio_set_direction(trig, GPIO_MODE_OUTPUT);
    gpio_set_direction(echo, GPIO_MODE_INPUT);

    gpio_set_level(en, 0);
    gpio_set_level(trig, 0);
}

void HC_SR04::set_config(uint32_t* config) {
    empty_from = *(config++);
    empty_to = *(config++);
}

#define TIMEOUT 6000
#define MAX_DISTANCE 4000
#define ROUNDTRIP_MM 5.8823529411764705882352941176471

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define ENTER_CRITICAL portENTER_CRITICAL(&mux)
#define EXIT_CRITICAL portEXIT_CRITICAL(&mux)

float HC_SR04::roundtrip() {
    float distance = -3;

    ENTER_CRITICAL;
        gpio_set_level(trig, 0);
        ets_delay_us(4);
        gpio_set_level(trig, 1);
        ets_delay_us(10);
        gpio_set_level(trig, 0);

        if (gpio_get_level(echo)) {
            EXIT_CRITICAL;
            ESP_LOGE(TAG, "(Cannot ping)");
            return -1;
        }

        int64_t start = esp_timer_get_time();
        while (!gpio_get_level(echo)) {
            if (esp_timer_get_time() - start >= TIMEOUT) {
                EXIT_CRITICAL;
                ESP_LOGE(TAG, "(Ping timeout)");
                return -2;
            }
        }

        int64_t echo_start = esp_timer_get_time();
        int64_t echo_end = echo_start;
        while (gpio_get_level(echo)) {
            echo_end = esp_timer_get_time();
            distance = (float)(echo_end - echo_start) / ROUNDTRIP_MM;
            if (distance >= MAX_DISTANCE) {
                EXIT_CRITICAL;
                ESP_LOGE(TAG, "(Echo timeout)");
                return MAX_DISTANCE;
            }
        }
    EXIT_CRITICAL;

    return distance;
}

bool HC_SR04::is_out_of_range(float mm) {
    return mm < 0
        ? false
        : !(empty_from <= mm && mm <= empty_to);
}

uint8_t HC_SR04::measure() {
    uint8_t success = 0;

    gpio_set_level(en, 1);

    for (uint8_t i = 0; i < CONFIG_MEASURE_ITERATIONS - CONFIG_MEASURE_SUCCESS + 1 + success; i++) {
        float roundtrip = this->roundtrip();

        if (i >= CONFIG_MEASURE_DISCARD) {
            ESP_LOGI(TAG, "roundtrip => %6.2f mm", roundtrip);

            if (this->is_out_of_range(roundtrip)) {
                if (++success >= CONFIG_MEASURE_SUCCESS) {
                    break;
                }
            } else {
                success = 0;
            }

        } else {
            ESP_LOGI(TAG, "roundtrip => %6.2f mm, discarded", roundtrip);
        }
        
        light_sleep(CONFIG_MEASURE_DELAY);
    }
    
    gpio_set_level(en, 0);
    
    return success;
}

HC_SR04 sensor(CONFIG_ULTRASONIC_GPIO_EN, CONFIG_ULTRASONIC_GPIO_TRIG, CONFIG_ULTRASONIC_GPIO_ECHO);

#endif
