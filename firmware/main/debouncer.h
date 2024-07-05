#ifndef _DEBOUNCER_h
#define _DEBOUNCER_h

#include <freertos/FreeRTOS.h>

typedef struct {
    uint32_t t, db, i;
    uint8_t f, v;
} Debouncer;

void debouncer_new(Debouncer* x, uint32_t time, uint8_t init);
uint8_t debouncer_tick(Debouncer* x, uint8_t b);

#endif