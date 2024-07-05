#ifndef _SLEEP_h
#define _SLEEP_h

#include <freertos/FreeRTOS.h>

void light_sleep(const uint32_t ms);
void deep_sleep(const uint32_t ms);

#endif