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
#include "battery.h"

#if (!defined(CONFIG_USE_ULTRASONIC) && !defined(CONFIG_USE_INFRARED)) || \
    (defined(CONFIG_USE_ULTRASONIC) && defined(CONFIG_USE_INFRARED))
    #error "Only one of CONFIG_USE_ULTRASONIC or CONFIG_USE_INFRARED must be defined"
#endif

#if defined(CONFIG_USE_ULTRASONIC)
    HC_SR04 sensor(CONFIG_ULTRASONIC_GPIO_TRIG, CONFIG_ULTRASONIC_GPIO_ECHO);
#elif defined(CONFIG_USE_INFRARED)
    Infrared sensor(CONFIG_INFRARED_GPIO_TX_EN, CONFIG_INFRARED_GPIO_RX_EN, CONFIG_INFRARED_ADC_RX);
#endif

RTC_DATA_ATTR uint8_t first_boot = 1;
RTC_DATA_ATTR uint8_t mail_state = 0;
RTC_DATA_ATTR uint8_t mail_notify = 0;

#if defined(CONFIG_USE_BATTERY_CHECK)
    Battery battery(CONFIG_BATTERY_CHECK_ADC);

    RTC_DATA_ATTR uint8_t battery_state = 0;
    RTC_DATA_ATTR uint8_t battery_notify = 0;
#endif

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

        bool has_mail = sensor.measure() > 0;
        mail_notify |= (!mail_state && has_mail);
        mail_state = has_mail;

        #if defined(CONFIG_USE_BATTERY_CHECK)
            bool battery_low = battery.measure() > 0;
            battery_notify |= (battery_state != battery_low);
            battery_state = battery_low;
        #endif

        if (mail_notify
            #if defined(CONFIG_USE_BATTERY_CHECK)
                || battery_notify
            #endif
        ) {
            wifi_start();
            if (mail_notify && api_notify()) {
                mail_notify = 0;
            }
            #if defined(CONFIG_USE_BATTERY_CHECK)
                if (battery_notify && api_battery(battery_state)) {
                    battery_notify = 0;
                }
            #endif
            wifi_stop();
        }
        
        status_led.sleeping();

        deep_sleep(CONFIG_SLEEP_INTERVAL * 1000);

        esp_task_wdt_reset();
    }
}
