#ifndef _API_h
#define _API_h

#include <freertos/FreeRTOS.h>

extern uint32_t* api_config();
extern bool api_notify();
extern bool api_battery(bool battery_low);

#endif
