#include "config.h"

#include <freertos/FreeRTOS.h>

#include "sleep.h"
#include "wifi.h"
#include "https_request.h"
#include "hc_sr04.h"
#include "infrared.h"
#include "debouncer.h"
#include "status_led.h"

/* 
   openssl s_client -showcerts -connect server:443 </dev/null
   The CA root cert is the last cert given in the chain of certs.
*/
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

inline int send_notification() {
    return https_get_request(
        CONFIG_NOTIFICATION_SERVER,
        CONFIG_NOTIFICATION_URL,
        server_root_cert_pem_start,
        server_root_cert_pem_end
    );
}

#if !defined(CONFIG_USE_ULTRASOUND) && !defined(CONFIG_USE_INFRARED)
    #error "At least one of CONFIG_USE_ULTRASOUND or CONFIG_USE_INFRARED must be defined"
#endif

#if defined(CONFIG_USE_ULTRASOUND)
HC_SR04 ultrasound(CONFIG_GPIO_ULTRASOUND_TRIG, CONFIG_GPIO_ULTRASOUND_ECHO);
#endif
#if defined(CONFIG_USE_INFRARED)
Infrared infrared(CONFIG_GPIO_INFRARED_EN, CONFIG_ADC_INFRARED_RECV);
#endif

RTC_DATA_ATTR Debouncer db;

RTC_DATA_ATTR uint8_t first_boot = 1;
RTC_DATA_ATTR uint8_t p = 0;
RTC_DATA_ATTR uint8_t notify = 0;

#if defined(CONFIG_USE_STATUS_LED)
StatusLED status_led(CONFIG_GPIO_STATUS_LED_R, CONFIG_GPIO_STATUS_LED_G, CONFIG_GPIO_STATUS_LED_B);
#endif

extern "C" void app_main() {
    #if defined(CONFIG_USE_STATUS_LED)
        status_led.booting();
    #endif

    wifi_init();

    if (first_boot) {
        first_boot = 0;
        debouncer_new(&db, CONFIG_MAIL_DEBOUNCE_ITERATIONS, 0);
    }

    while (1) {
        #if defined(CONFIG_USE_STATUS_LED)
            status_led.active();
        #endif

        for (uint8_t i = 0; !notify && i < CONFIG_MAIL_DEBOUNCE_ITERATIONS * 2; i++) {
            uint8_t x = 
                #if defined(CONFIG_USE_ULTRASOUND)
                    ultrasound.measure()

                    #if defined(CONFIG_USE_INFRARED)
                        ||
                    #endif
                #endif
                
                #if defined(CONFIG_USE_INFRARED)
                    infrared.measure()
                #endif
            ;

            x = debouncer_tick(&db, x);

            notify |= ~p & x;
            p = x;

            light_sleep(CONFIG_MAIL_SENSOR_POLL_DELAY);
        }
        
        if (notify) {
            if (wifi_start() && send_notification()) {
                notify = 0;
            }
            wifi_stop();
        }
        
        #if defined(CONFIG_USE_STATUS_LED)
            status_led.sleeping();
        #endif

        deep_sleep(CONFIG_SLEEP_INTERVAL * 1000);
    }
}
