#ifndef _HTTPS_REQUEST_h
#define _HTTPS_REQUEST_h

#include <freertos/FreeRTOS.h>

int https_get_request(const char* server, const char* url, const uint8_t* cacert_start, const uint8_t* cacert_end);

#endif