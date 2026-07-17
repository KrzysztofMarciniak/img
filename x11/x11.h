#ifndef X11_H
#define X11_H

#include <stdbool.h>

#include "backend/backend.h"

typedef enum {
    EVENT_REDRAW,
    EVENT_NEXT,
    EVENT_PREV,
    EVENT_QUIT,
} Event;

bool x11_init(int width, int height);
void x11_destroy(void);

void x11_draw(const Image *img);

Event x11_wait_event(void);

#endif

