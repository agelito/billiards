#ifndef KEYBOARD_X11_H_INCLUDED
#define KEYBOARD_X11_H_INCLUDED

#define MAX_KEYCODE_COUNT 256
#define MAX_KEYSYM_PER_KEYCODE 16

typedef struct
{
    int symbol_count;
    KeySym symbols[MAX_KEYSYM_PER_KEYCODE];
} keysym_list;

typedef struct
{
    int keycode_min;
    int keycode_max;
    int keycode_count;

    keysym_list symbol_list[MAX_KEYCODE_COUNT];
} keycode_map;

keycode_map create_keycode_map(Display* display);

int keycode_is_symbol(keycode_map* keyboard_map, int keycode, KeySym symbol);

#endif // KEYBOARD_X11_H_INCLUDED
