#ifndef MOUSE_H_INCLUDED
#define MOUSE_H_INCLUDED

typedef struct
{
    int absolute_x;
    int absolute_y;
    double relative_x;
    double relative_y;
    double absolute_scroll;
    double relative_scroll;
} mouse_state;

void mouse_apply_relative(mouse_state* mouse, double rel_x, double rel_y, double rel_scroll);
void mouse_clamp_to_window(mouse_state* mouse, int window_width, int window_height);
void mouse_wrap_window(mouse_state* mouse, int window_width, int window_height);

#endif // MOUSE_H_INCLUDED
