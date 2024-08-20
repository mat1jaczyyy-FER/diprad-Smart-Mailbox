#ifndef _API_h
#define _API_h

#include <freertos/FreeRTOS.h>

uint32_t* api_config();
bool api_notify();
bool api_battery(uint8_t battery_low);

#endif
