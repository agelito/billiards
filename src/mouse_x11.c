// mouse_x11.c

#include <X11/Xlib.h>
#include "mouse_x11.h"

mouse_input
init_mouse_input(Display* display, Window window)
{
    mouse_input input = (mouse_input){0};

    int mouse_x, mouse_y;
    mouse_get_position(display, window, &mouse_x, &mouse_y);

    input.position_x = mouse_x;
    input.position_y = mouse_y;

    mouse_grab(display, window);
    
    return input;
}

void
mouse_get_position(Display* display, Window window, int* position_x, int* position_y)
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
mouse_set_position(Display* display, Window window, int position_x, int position_y)
{
    XWarpPointer(display, None, window, 0, 0, 0, 0, position_x, position_y);
}

void
mouse_grab(Display* display, Window window)
{
    XGrabPointer(display, window, True, None, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);
}

void
mouse_ungrab(Display* display)
{
    XUngrabPointer(display, CurrentTime);
}


