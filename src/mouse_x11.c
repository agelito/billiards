// mouse_x11.c

#include <X11/Xlib.h>
#include "mouse_x11.h"

mouse_input
init_mouse_input(Display* display, Window window)
{
    mouse_input input = (mouse_input){0};

    Window root_window;
    Window child_window;

    int root_x, root_y, window_x, window_y;
    unsigned int mask;
    if(XQueryPointer(display, window, &root_window, &child_window, &root_y, &root_x, &window_x, &window_y, &mask))
    {
	input.relative_x = window_x;
	input.relative_y = window_y;
    }
    
    return input;
}
