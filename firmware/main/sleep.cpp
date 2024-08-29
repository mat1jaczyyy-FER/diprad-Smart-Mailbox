#include "config.h"
#include "sleep.h"

#include <time.h>
#include <sys/time.h>

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

RTC_DATA_ATTR uint32_t boot_seconds_of_day;

void mailbox_sleep_set_seconds_of_day(const uint32_t seconds_of_day) {
    struct timeval boot_time;
    gettimeofday(&boot_time, NULL);

    boot_seconds_of_day = (seconds_of_day - boot_time.tv_sec + 86400) % 86400;
}

int calc_mailbox_sleep_time(const uint32_t now) {
    if (now <= 3 * 60 * 60) {
        return 3 * 60 * 60;
    }
    if (now <= 4 * 60 * 60) {
        return 2 * 60 * 60;
    }
    if (now <= 5 * 60 * 60) {
        return 60 * 60;
    }
    if (now <= 7 * 60 * 60) {
        return 30 * 60;
    }
    if (now <= 8 * 60 * 60) {
        return 20 * 60;
    }
    if (now <= 12 * 60 * 60) {
        return 10 * 60;
    }
    if (now <= 15 * 60 * 60) {
        return 20 * 60;
    }
    if (now <= 18 * 60 * 60) {
        return 30 * 60;
    }
    if (now <= 20 * 60 * 60) {
        return 60 * 60;
    }
    if (now <= 21 * 60 * 60) {
        return 2 * 60 * 60;
    }
    return 3 * 60 * 60;
}

void mailbox_sleep() {
    #ifndef CONFIG_SLEEP_TEST_INTERVAL
        struct timeval sleep_enter_time;
        gettimeofday(&sleep_enter_time, NULL);

        uint32_t now = (boot_seconds_of_day + (sleep_enter_time.tv_sec % 86400)) % 86400;
    #endif

    deep_sleep(
        #ifdef CONFIG_SLEEP_TEST_INTERVAL
            CONFIG_SLEEP_TEST_INTERVAL
        #else
            calc_mailbox_sleep_time(now)
         #endif
        * 1000
    );
}
