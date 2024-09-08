#ifndef _SLEEP_h
#define _SLEEP_h

#include <freertos/FreeRTOS.h>

extern void light_sleep(const uint32_t ms);
extern void deep_sleep(const uint32_t ms);

extern void mailbox_sleep_set_seconds_of_day(const uint32_t seconds_of_day);
extern void mailbox_sleep();

#endif
