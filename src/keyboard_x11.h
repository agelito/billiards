#ifndef KEYBOARD_X11_H_INCLUDED
#define KEYBOARD_X11_H_INCLUDED

#include "keyboard.h"

#define MAX_KEYCODE_COUNT 256
#define MAX_KEYSYM_PER_KEYCODE 16

typedef struct
{
    int symbol_count;
    KeySym symbols[MAX_KEYSYM_PER_KEYCODE];
} keysym_list;

typedef struct
{
    int down;
    int pressed;
    int released;
} keycode_state;

typedef struct
{
    int keycode_min;
    int keycode_max;
    int keycode_count;
    
    keysym_list map[MAX_KEYCODE_COUNT];
    keycode_state state[MAX_KEYCODE_COUNT];

    int vkey_to_keycode[VKEY_COUNT];
} keyboard_x11;

keyboard_x11 keyboard_x11_init(Display* display);

void keyboard_x11_reset(keyboard_x11* keyboard);

int keyboard_x11_is_symbol(keyboard_x11* keyboard, int keycode, KeySym symbol);
int keyboard_x11_is_down(keyboard_x11* keyboard, virtual_key key);
int keyboard_x11_is_pressed(keyboard_x11* keyboard, virtual_key key);
int keyboard_x11_is_released(keyboard_x11* keyboard, virtual_key key);

#endif // KEYBOARD_X11_H_INCLUDED
