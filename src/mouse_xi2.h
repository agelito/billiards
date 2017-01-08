#ifndef MOUSE_XI2_H_INCLUDED
#define MOUSE_XI2_H_INCLUDED

#define MOUSE_XI2_MAX_AXIS 16

typedef struct
{
    int major_opcode;
    int selected_device;

    int axis_count;
    double axis_raw_data[MOUSE_XI2_MAX_AXIS];
} xinput2;

typedef struct
{
    double x;
    double y;
    double wheel;
} xinput2_mouse;

xinput2 xinput2_init(Display* display);

int xinput2_process_event(Display* display, xinput2* input, XEvent* event);
void xinput2_reset_axis_data(xinput2* input);

xinput2_mouse xinput2_get_default_axis(xinput2* input);

#endif // MOUSE_XI2_H_INCLUDED
