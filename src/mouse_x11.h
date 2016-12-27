#ifndef MOUSE_X11_H_INCLUDED
#define MOUSE_X11_H_INCLUDED

typedef struct
{
    int relative_x;
    int relative_y;
    int movement_delta_x;
    int movement_delta_y;
} mouse_input;

mouse_input init_mouse_input(Display* display, Window window);


#endif // MOUSE_X11_H_INCLUDED
