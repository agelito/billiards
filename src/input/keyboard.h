#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

typedef enum
{
    VKEY_A, VKEY_B, VKEY_C, VKEY_D,
    VKEY_E, VKEY_F, VKEY_G, VKEY_H,
    VKEY_I, VKEY_J, VKEY_K, VKEY_L,
    VKEY_M, VKEY_N, VKEY_O, VKEY_P,
    VKEY_Q, VKEY_R, VKEY_S, VKEY_T,
    VKEY_U, VKEY_V, VKEY_W, VKEY_X,
    VKEY_Y, VKEY_Z, VKEY_ESCAPE,
    VKEY_COUNT
} virtual_key;

typedef struct
{
    int down[VKEY_COUNT];
    int pressed[VKEY_COUNT];
    int released[VKEY_COUNT];
} keyboard_state;

int keyboard_is_down(keyboard_state* keyboard, virtual_key key);
int keyboard_is_pressed(keyboard_state* keyboard, virtual_key key);
int keyboard_is_released(keyboard_state* keyboard, virtual_key key);

#endif // KEYBOARD_H_INCLUDED
