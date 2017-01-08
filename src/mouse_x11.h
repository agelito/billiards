#ifndef MOUSE_X11_H_INCLUDED
#define MOUSE_X11_H_INCLUDED

void mouse_x11_get_position(Display* display, Window window, int* position_x, int* position_y);

void mouse_x11_set_position(Display* display, Window window, int position_x, int position_y);

int mouse_x11_grab(Display* display, Window window);

void mouse_x11_ungrab(Display* display);

void mouse_x11_set_cursor(Display* display, Cursor cursor);

#endif // MOUSE_X11_H_INCLUDED
