// mouse.c

#include "mouse.h"

void
mouse_apply_relative(mouse_state* mouse, double rel_x, double rel_y, double rel_scroll)
{
    mouse->absolute_x = (mouse->absolute_x + rel_x);
    mouse->absolute_y = (mouse->absolute_y + rel_y);
    mouse->relative_x = rel_x;
    mouse->relative_y = rel_y;
    mouse->absolute_scroll += rel_scroll;
    mouse->relative_scroll = rel_scroll;
}

void
mouse_clamp_to_window(mouse_state* mouse, int window_width, int window_height)
{
    if(mouse->absolute_x < 0) mouse->absolute_x = 0;
    if(mouse->absolute_x >= window_width) mouse->absolute_x = window_width - 1;
    
    if(mouse->absolute_y < 0) mouse->absolute_y = 0;
    if(mouse->absolute_y >= window_height) mouse->absolute_y = window_height - 1;
}

void
mouse_wrap_window(mouse_state* mouse, int window_width, int window_height)
{
    while(mouse->absolute_x < 0) mouse->absolute_x += window_width;
    while(mouse->absolute_x >= window_width) mouse->absolute_x -= window_width;

    while(mouse->absolute_y < 0) mouse->absolute_y += window_height;
    while(mouse->absolute_y >= window_width) mouse->absolute_y -= window_height;
}
