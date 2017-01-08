// mouse_x11.c

#include <X11/Xlib.h>
#include "mouse_x11.h"

void
mouse_x11_get_position(Display* display, Window window, int* position_x, int* position_y)
{
    Window root_window;
    Window child_window;

    int root_x, root_y, window_x, window_y;
    unsigned int mask;
    
    XQueryPointer(display, window, &root_window, &child_window, &root_y, &root_x, &window_x, &window_y, &mask);

    *position_x = window_x;
    *position_y = window_y;
}

void
mouse_x11_set_position(Display* display, Window window, int position_x, int position_y)
{
    XWarpPointer(display, None, window, 0, 0, 0, 0, position_x, position_y);
}

int
mouse_x11_grab(Display* display, Window window)
{
    int grab_status = XGrabPointer(display, window, True, None, GrabModeAsync, GrabModeAsync,
				   window, None, CurrentTime);
    
    return (grab_status == Success); 
}

void
mouse_x11_ungrab(Display* display)
{
    XUngrabPointer(display, CurrentTime);
}

void
mouse_x11_set_cursor(Display* display, Cursor cursor)
{
    Window window = DefaultRootWindow(display);
    XDefineCursor(display, window, cursor);
}


