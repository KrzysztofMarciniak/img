#include "x11.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

static Display *display;
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
x11_draw(const Image *img)
{
    if (!img || !display)
        return;


    XImage *ximg = XCreateImage(
        display,
        DefaultVisual(display, screen),
        DefaultDepth(display, screen),
        ZPixmap,
        0,
        NULL,
        img->width,
        img->height,
        32,
        0);

    if (!ximg)
        return;


    ximg->data = malloc(ximg->bytes_per_line * img->height);

    if (!ximg->data) {
        XDestroyImage(ximg);
        return;
    }


    for (int y = 0; y < img->height; y++) {

        uint32_t *dst =
            (uint32_t *)(ximg->data +
                         y * ximg->bytes_per_line);

        const uint8_t *src =
            img->rgba +
            y * img->width * 4;


        for (int x = 0; x < img->width; x++) {

            uint8_t r = src[x * 4 + 0];
            uint8_t g = src[x * 4 + 1];
            uint8_t b = src[x * 4 + 2];

            dst[x] =
                ((uint32_t)r << 16) |
                ((uint32_t)g << 8)  |
                b;
        }
    }

XWindowAttributes attr;

XGetWindowAttributes(display, window, &attr);

int x = (attr.width  - img->width)  / 2;
int y = (attr.height - img->height) / 2;

if (x < 0)
    x = 0;

if (y < 0)
    y = 0;


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
    img->width,
    img->height);


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
            KeySym key =
                XLookupKeysym(&ev.xkey, 0);

            switch (key) {

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

