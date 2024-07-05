#include "config.h"
#include "hc_sr04.h"

#include <esp_log.h>

static const char* TAG = "HC_SR04";

#if defined(CONFIG_USE_ULTRASOUND) && !defined(CONFIG_MAIL_ULTRASOUND_EMPTY_FROM) && !defined(CONFIG_MAIL_ULTRASOUND_EMPTY_TO)
    #error "At least one of CONFIG_MAIL_ULTRASOUND_EMPTY_FROM or CONFIG_MAIL_ULTRASOUND_EMPTY_TO must be defined"
#endif

HC_SR04::HC_SR04(gpio_num_t _trig, gpio_num_t _echo) {
    trig = _trig;
    echo = _echo;

    gpio_set_direction(trig, GPIO_MODE_OUTPUT);
    gpio_set_direction(echo, GPIO_MODE_INPUT);

    gpio_set_level(trig, 0);
}

#define TIMEOUT 6000
#define MAX_DISTANCE 400
#define ROUNDTRIP_CM 58.823529411764705882352941176471

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define ENTER_CRITICAL portENTER_CRITICAL(&mux)
#define EXIT_CRITICAL portEXIT_CRITICAL(&mux)

uint8_t HC_SR04::measure() {
    ENTER_CRITICAL;
        gpio_set_level(trig, 0);
        ets_delay_us(4);
        gpio_set_level(trig, 1);
        ets_delay_us(10);
        gpio_set_level(trig, 0);

        if (gpio_get_level(echo)) {
            EXIT_CRITICAL;
            ESP_LOGE(TAG, "Cannot ping");
            return -1;
        }

        int64_t start = esp_timer_get_time();
        while (!gpio_get_level(echo)) {
            if (esp_timer_get_time() - start >= TIMEOUT) {
                EXIT_CRITICAL;
                ESP_LOGE(TAG, "Ping timeout");
                return 0;
            }
        }

        int64_t echo_start = esp_timer_get_time();
        int64_t echo_end = echo_start;
        while (gpio_get_level(echo)) {
            echo_end = esp_timer_get_time();
            if (echo_end - echo_start >= MAX_DISTANCE * ROUNDTRIP_CM) {
                EXIT_CRITICAL;
                ESP_LOGI(TAG, "%6.2f cm (Echo timeout)", MAX_DISTANCE);
                return 0;
            }
        }
    EXIT_CRITICAL;

    float result = (float)(echo_end - echo_start) / ROUNDTRIP_CM;
    ESP_LOGI(TAG, "%6.2f cm", result);
    
    return !(
        #if defined(CONFIG_MAIL_ULTRASOUND_EMPTY_FROM)
            CONFIG_MAIL_ULTRASOUND_EMPTY_FROM <= result

            #if defined(CONFIG_MAIL_ULTRASOUND_EMPTY_TO)
                &&
            #endif
        #endif
          
        #if defined(CONFIG_MAIL_ULTRASOUND_EMPTY_TO)
            result <= CONFIG_MAIL_ULTRASOUND_EMPTY_TO
        #endif
    );
}
