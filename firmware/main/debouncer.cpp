#include "debouncer.h"

void debouncer_new(Debouncer* x, uint32_t time, uint8_t init) {
    *x = { 0, time, 0, 0, init };
}

uint8_t debouncer_tick(Debouncer* x, uint8_t b) {
    x->i++;
    if (!x->f) {
        if (b != x->v) {
            x->f = 1;
            x->t = x->i;
        }
    } else {
        if (b != x->v) {
            if (x->i - x->t >= x->db) {
                x->f = 0;
                x->v = b;
            }
        } else {
            x->f = 0;
        }
    }
    return x->v;
}
