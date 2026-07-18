#include "x11.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <stdint.h>

static Display *display = NULL;
static Window window;
static GC gc;
static int screen;

bool
x11_init(int width, int height)
{
    display = XOpenDisplay(NULL);

    if (!display)
        return false;

    screen = DefaultScreen(display);

    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        0,
        0,
        width,
        height,
        0,
        BlackPixel(display, screen),
        BlackPixel(display, screen));

    XSelectInput(
        display,
        window,
        ExposureMask |
        KeyPressMask |
        StructureNotifyMask);

    gc = XCreateGC(display, window, 0, NULL);

    XMapWindow(display, window);

    return true;
}

void
x11_destroy(void)
{
    if (!display)
        return;

    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    display = NULL;
}

void
x11_draw(const Image *img, double zoom, int pan_x, int pan_y)
{
    if (!img || !display)
        return;

    int dst_w = (int)(img->width  * zoom);
    int dst_h = (int)(img->height * zoom);
    if (dst_w < 1)
        dst_w = 1;
    if (dst_h < 1)
        dst_h = 1;

    XImage *ximg = XCreateImage(
        display,
        DefaultVisual(display, screen),
        DefaultDepth(display, screen),
        ZPixmap,
        0,
        NULL,
        dst_w,
        dst_h,
        32,
        0);
    if (!ximg)
        return;
    ximg->data = malloc(ximg->bytes_per_line * dst_h);
    if (!ximg->data) {
        XDestroyImage(ximg);
        return;
    }
    for (int y = 0; y < dst_h; y++) {
        int sy = (int)(y / zoom);
        if (sy >= img->height)
            sy = img->height - 1;
        uint32_t *dst =
            (uint32_t *)(ximg->data +
                         y * ximg->bytes_per_line);
        const uint8_t *src =
            img->rgba +
            sy * img->width * 4;
        for (int x = 0; x < dst_w; x++) {
            int sx = (int)(x / zoom);
            if (sx >= img->width)
                sx = img->width - 1;
            uint8_t r = src[sx * 4 + 0];
            uint8_t g = src[sx * 4 + 1];
            uint8_t b = src[sx * 4 + 2];
            dst[x] =
                ((uint32_t)r << 16) |
                ((uint32_t)g << 8)  |
                b;
        }
    }

    XWindowAttributes attr;
    XGetWindowAttributes(display, window, &attr);
    int x = (attr.width  - dst_w) / 2 + pan_x;
    int y = (attr.height - dst_h) / 2 + pan_y;

    XClearWindow(display, window);
    XPutImage(
        display,
        window,
        gc,
        ximg,
        0,
        0,
        x,
        y,
        dst_w,
        dst_h);
    XFlush(display);
    XDestroyImage(ximg);
}

Event
x11_wait_event(void)
{
    XEvent ev;
    for (;;) {
        XNextEvent(display, &ev);
        switch (ev.type) {
        case Expose:
            return EVENT_REDRAW;
        case KeyPress: {
            unsigned int idx = (ev.xkey.state & ShiftMask) ? 1 : 0;
            KeySym key = XLookupKeysym(&ev.xkey, idx);
            switch (key) {
            case XK_H:
                return EVENT_PREV;
            case XK_L:
                return EVENT_NEXT;
            case XK_h:
                return EVENT_PAN_LEFT;
            case XK_l:
                return EVENT_PAN_RIGHT;
            case XK_k:
                return EVENT_PAN_UP;
            case XK_j:
                return EVENT_PAN_DOWN;
            case XK_plus:
            case XK_equal:
                return EVENT_ZOOM_IN;
            case XK_minus:
                return EVENT_ZOOM_OUT;
            case XK_0:
                return EVENT_ZOOM_RESET;
            case XK_q:
            case XK_Escape:
                return EVENT_QUIT;
            default:
                break;
            }
            break;
        }
        case DestroyNotify:
            return EVENT_QUIT;
        }
    }
}
