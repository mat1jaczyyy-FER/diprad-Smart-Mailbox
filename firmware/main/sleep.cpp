#include "config.h"
#include "sleep.h"

#include <freertos/task.h>

#include "esp_sleep.h"

void light_sleep(const uint32_t ms) {
    #if CONFIG_SLEEP_MAXLEVEL >= 1
    esp_sleep_enable_timer_wakeup(ms * 1000LL);

    if (esp_light_sleep_start() == ESP_ERR_SLEEP_TOO_SHORT_SLEEP_DURATION) {
    #endif
        vTaskDelay(ms / portTICK_PERIOD_MS);
    #if CONFIG_SLEEP_MAXLEVEL >= 1
    }
    #endif
}

void deep_sleep(const uint32_t ms) {
    #if CONFIG_SLEEP_MAXLEVEL >= 2
    esp_sleep_enable_timer_wakeup(ms * 1000LL);
    esp_deep_sleep_start();
    #else
    light_sleep(ms);
    #endif
}
