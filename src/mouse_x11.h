#ifndef MOUSE_X11_H_INCLUDED
#define MOUSE_X11_H_INCLUDED

typedef struct
{
    int position_x;
    int position_y;
    int movement_delta_x;
    int movement_delta_y;
} mouse_input;

mouse_input init_mouse_input(Display* display, Window window);

void mouse_get_position(Display* display, Window window, int* position_x, int* position_y);
void mouse_set_position(Display* display, Window window, int position_x, int position_y);

void mouse_grab(Display* display, Window window);
void mouse_ungrab(Display* display);

#endif // MOUSE_X11_H_INCLUDED
