#include "config.h"

#include <freertos/FreeRTOS.h>
#include "esp_log.h"

#include "sleep.h"
#include "wifi.h"
#include "api.h"
#include "hc_sr04.h"
#include "infrared.h"
#include "debouncer.h"
#include "status_led.h"

#if (!defined(CONFIG_USE_ULTRASONIC) && !defined(CONFIG_USE_INFRARED)) || \
    (defined(CONFIG_USE_ULTRASONIC) && defined(CONFIG_USE_INFRARED))
    #error "Only of CONFIG_USE_ULTRASONIC or CONFIG_USE_INFRARED must be defined"
#endif

#if defined(CONFIG_USE_ULTRASONIC)
    HC_SR04 sensor(CONFIG_GPIO_ULTRASONIC_TRIG, CONFIG_GPIO_ULTRASONIC_ECHO);
#elif defined(CONFIG_USE_INFRARED)
    Infrared sensor(CONFIG_GPIO_INFRARED_EN, CONFIG_ADC_INFRARED_RECV);
#endif

#if !(defined(CONFIG_MAIL_EMPTY_FROM) && defined(CONFIG_MAIL_EMPTY_TO))
    #error "CONFIG_MAIL_EMPTY_FROM and CONFIG_MAIL_EMPTY_TO must be defined"
#endif

RTC_DATA_ATTR Debouncer db;

RTC_DATA_ATTR uint8_t first_boot = 1;
RTC_DATA_ATTR uint8_t p = 0;
RTC_DATA_ATTR uint8_t notify = 0;

extern "C" void app_main() {
    status_led.active();
    
    wifi_init();

    if (first_boot) {
        first_boot = 0;
        debouncer_new(&db, CONFIG_MAIL_DEBOUNCE_ITERATIONS, 0);

        wifi_start();

        char* config = api_config();
        if (config) {
            ESP_LOGI("MAIN", "Config received: %s", config);
            free(config);
        }

        wifi_stop();
    }

    while (1) {
        status_led.active();

        for (uint8_t i = 0; !notify && i < CONFIG_MAIL_DEBOUNCE_ITERATIONS * 2; i++) {
            uint8_t x = sensor.measure();

            x = debouncer_tick(&db, x);

            notify |= ~p & x;
            p = x;

            light_sleep(CONFIG_MAIL_SENSOR_POLL_DELAY);
        }
        
        if (notify) {
            wifi_start();
            if (api_notify()) {
                notify = 0;
            }
            wifi_stop();
        }
        
        status_led.sleeping();

        deep_sleep(CONFIG_SLEEP_INTERVAL * 1000);
    }
}
