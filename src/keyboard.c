// keyboard.c

#include "keyboard.h"

int
keyboard_is_down(keyboard_state* keyboard, virtual_key key)
{
    return *(keyboard->down + key);
}

int
keyboard_is_pressed(keyboard_state* keyboard, virtual_key key)
{
    return *(keyboard->pressed + key);
}

int
keyboard_is_released(keyboard_state* keyboard, virtual_key key)
{
    return *(keyboard->released + key);
}
