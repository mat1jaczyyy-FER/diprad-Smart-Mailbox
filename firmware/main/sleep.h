#ifndef _SLEEP_h
#define _SLEEP_h

#include <freertos/FreeRTOS.h>

void light_sleep(const uint32_t ms);
void deep_sleep(const uint32_t ms);

void mailbox_sleep_set_seconds_of_day(const uint32_t seconds_of_day);
void mailbox_sleep();

#endif
