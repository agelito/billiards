#ifndef KEYBOARD_X11_H_INCLUDED
#define KEYBOARD_X11_H_INCLUDED

#define MAX_KEYCODE_COUNT 256
#define MAX_KEYSYM_PER_KEYCODE 16

typedef enum
{
    VKEY_A,
    VKEY_B,
    VKEY_C,
    VKEY_D,
    VKEY_E,
    VKEY_F,
    VKEY_G,
    VKEY_H,
    VKEY_I,
    VKEY_J,
    VKEY_K,
    VKEY_L,
    VKEY_M,
    VKEY_N,
    VKEY_O,
    VKEY_P,
    VKEY_Q,
    VKEY_R,
    VKEY_S,
    VKEY_T,
    VKEY_U,
    VKEY_V,
    VKEY_W,
    VKEY_X,
    VKEY_Y,
    VKEY_Z,
    VKEY_ESCAPE,
    VKEY_COUNT
} virtual_key;

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
} keyboard_input;

keyboard_input keyboard_init(Display* display);
void keyboard_reset_state(keyboard_input* keyboard);

int keycode_is_symbol(keyboard_input* keyboard, int keycode, KeySym symbol);

int is_down(keyboard_input* keyboard, virtual_key key);
int is_pressed(keyboard_input* keyboard, virtual_key key);
int is_released(keyboard_input* keyboard, virtual_key key);

#endif // KEYBOARD_X11_H_INCLUDED
