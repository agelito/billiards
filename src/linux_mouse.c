// linux_mouse.c

static int
mouse_x11_grab(Display* display, Window window)
{
    int grab_status = XGrabPointer(display, window, True, None, GrabModeAsync, GrabModeAsync,
				   window, None, CurrentTime);
    
    return (grab_status == Success); 
}

static void
mouse_x11_ungrab(Display* display)
{
    XUngrabPointer(display, CurrentTime);
}

static void
mouse_x11_set_cursor(Display* display, Cursor cursor)
{
    Window window = DefaultRootWindow(display);
    XDefineCursor(display, window, cursor);
}


