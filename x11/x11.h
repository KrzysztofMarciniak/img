#ifndef X11_H
#define X11_H

#include <stdbool.h>

#include "backend/backend.h"

typedef enum {
    EVENT_REDRAW,
    EVENT_NEXT,
    EVENT_PREV,
    EVENT_ZOOM_IN,
    EVENT_ZOOM_OUT,
    EVENT_ZOOM_RESET,
    EVENT_PAN_LEFT,
    EVENT_PAN_RIGHT,
    EVENT_PAN_UP,
    EVENT_PAN_DOWN,
    EVENT_QUIT,
} Event;

bool x11_init(int width, int height);
void x11_destroy(void);

void x11_draw(const Image *img, double zoom, int pan_x, int pan_y);

Event x11_wait_event(void);

#endif
