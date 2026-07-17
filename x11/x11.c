#include "x11.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

static Display *display;
static Window window;
static GC gc;

bool
x11_init(int width, int height)
{
    display = XOpenDisplay(NULL);
    if (!display)
        return false;

    int screen = DefaultScreen(display);

    window = XCreateSimpleWindow(
        display,
        RootWindow(display, screen),
        0, 0,
        width, height,
        0,
        BlackPixel(display, screen),
        WhitePixel(display, screen));

    XSelectInput(display, window,
        ExposureMask |
        KeyPressMask |
        StructureNotifyMask);

    gc = XCreateGC(display, window, 0, NULL);

    XMapWindow(display, window);

    /* Wait until the window is actually mapped. */
    for (;;) {
        XEvent ev;
        XNextEvent(display, &ev);
        if (ev.type == MapNotify)
            break;
    }

    return true;
}


void
x11_destroy(void)
{
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void
x11_draw(const Image *img)
{
    if (!img)
        return;

    XResizeWindow(display, window,
                  img->width,
                  img->height);

    /*
     * Upload RGBA pixels here later with
     * XCreateImage()
     * XPutImage()
     */
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
            KeySym key = XLookupKeysym(&ev.xkey, 0);

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

