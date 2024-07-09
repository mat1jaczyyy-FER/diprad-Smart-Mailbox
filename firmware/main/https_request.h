#ifndef _HTTPS_REQUEST_h
#define _HTTPS_REQUEST_h

#include <freertos/FreeRTOS.h>

bool https_get_request(const char* server, const char* url, char** res, const uint8_t* cacert_start, const uint8_t* cacert_end);
bool https_post_request(const char* server, const char* url, const char* body, char** res, const uint8_t* cacert_start, const uint8_t* cacert_end);

#endif