#include "config.h"

#if (!defined(CONFIG_USE_ULTRASONIC) && !defined(CONFIG_USE_INFRARED)) || \
    (defined(CONFIG_USE_ULTRASONIC) && defined(CONFIG_USE_INFRARED))
    #error "Only one of CONFIG_USE_ULTRASONIC or CONFIG_USE_INFRARED must be defined"
#endif

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

static const char* TAG = "MAIN";

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
    wifi_init();

    if (first_boot) {
        first_boot = 0;
        status_led.active();

        wifi_start();

        uint32_t* config = api_config();
        
        mailbox_sleep_set_seconds_of_day(*config);
        sensor.set_config(config + 1);

        free(config);

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

        #ifdef CONFIG_TEST_DISABLE_NOTIFY
            if (mail_notify) {
                ESP_LOGW(TAG, "Mail detected!");
                mail_notify = 0;
            }
            #if defined(CONFIG_USE_BATTERY_CHECK)
                if (battery_notify) {
                    ESP_LOGW(TAG, "Battery is %s", battery_state? "low" : "good");
                    battery_notify = 0;
                }
            #endif
            ESP_LOGI(TAG, "");

        #else
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
        #endif

        status_led.sleeping();
        mailbox_sleep();

        esp_task_wdt_reset();
    }
}
