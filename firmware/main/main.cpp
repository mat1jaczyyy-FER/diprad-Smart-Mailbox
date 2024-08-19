#include "config.h"

#include <freertos/FreeRTOS.h>
#include "esp_log.h"
#include "esp_task_wdt.h"

#include "sleep.h"
#include "wifi.h"
#include "api.h"
#include "hc_sr04.h"
#include "infrared.h"
#include "status_led.h"

#if (!defined(CONFIG_USE_ULTRASONIC) && !defined(CONFIG_USE_INFRARED)) || \
    (defined(CONFIG_USE_ULTRASONIC) && defined(CONFIG_USE_INFRARED))
    #error "Only one of CONFIG_USE_ULTRASONIC or CONFIG_USE_INFRARED must be defined"
#endif

#if defined(CONFIG_USE_ULTRASONIC)
    HC_SR04 sensor(CONFIG_GPIO_ULTRASONIC_TRIG, CONFIG_GPIO_ULTRASONIC_ECHO);
#elif defined(CONFIG_USE_INFRARED)
    Infrared sensor(CONFIG_GPIO_INFRARED_EN_TX, CONFIG_GPIO_INFRARED_EN_RX, CONFIG_ADC_INFRARED_RX);
#endif

RTC_DATA_ATTR uint8_t first_boot = 1;
RTC_DATA_ATTR uint8_t p = 0;
RTC_DATA_ATTR uint8_t notify = 0;

extern "C" void app_main() {
    status_led.init();
    status_led.active();
    
    wifi_init();

    if (first_boot) {
        first_boot = 0;

        wifi_start();

        char* config = api_config();
        if (config) {
            ESP_LOGI("MAIN", "Config received: \n\n%s\n", config);
            free(config);
        }

        wifi_stop();
    }

    while (1) {
        status_led.active();

        bool x = sensor.measure() > 0;

        notify |= ~p & x;
        p = x;
        
        if (notify) {
            wifi_start();
            if (api_notify()) {
                notify = 0;
            }
            wifi_stop();
        }
        
        status_led.sleeping();

        deep_sleep(CONFIG_SLEEP_INTERVAL * 1000);

        esp_task_wdt_reset();
    }
}
