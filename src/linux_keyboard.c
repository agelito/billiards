// linux_keyboard.c

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

static void
create_keycode_map(Display* display, keyboard_x11* keyboard)
{
    int keycode_min, keycode_max;
    XDisplayKeycodes(display, &keycode_min, &keycode_max);

    int keycode_count = (keycode_max - keycode_min);
    if(keycode_count > MAX_KEYCODE_COUNT)
    {
	printf("warning: keycode_count(%d) > MAX_KEYCODE_COUNT(%d)\n", keycode_count, MAX_KEYCODE_COUNT);
	keycode_count = MAX_KEYCODE_COUNT;
	keycode_max = (keycode_min + keycode_count);
    }

    keyboard->keycode_min = keycode_min;
    keyboard->keycode_max = keycode_max;
    keyboard->keycode_count = keycode_count;

    int keysym_per_keycode;
    KeySym* keyboard_mapping = XGetKeyboardMapping(display, keycode_min, keycode_count, &keysym_per_keycode);
    if(keyboard_mapping != 0)
    {
	if(keysym_per_keycode > MAX_KEYSYM_PER_KEYCODE)
	{
	    printf("warning: keysym_per_keycode(%d) > MAX_KEYSYM_PER_KEYCODE(%d)\n",
		   keysym_per_keycode, MAX_KEYSYM_PER_KEYCODE);
	    keysym_per_keycode = MAX_KEYSYM_PER_KEYCODE;
	}
	
	int keycode_index;
	keysym_list* symbols;
	for(keycode_index = 0; keycode_index < keycode_count; ++keycode_index)
	{
	    symbols = (keyboard->map + keycode_index);

	    KeySym* keycode_symbols = (keyboard_mapping + (keycode_index * keysym_per_keycode));

	    int keysym_index, keysym_count = 0;
	    for(keysym_index = 0; keysym_index < keysym_per_keycode; ++keysym_index)
	    {
		KeySym keysym = *(keycode_symbols + keysym_index);
		if(keysym != 0)
		{
		    *(symbols->symbols + keysym_count++) = keysym;
		}
	    }
	    symbols->symbol_count = keysym_count;
	}
	
	XFree(keyboard_mapping);
    }
}

static bool32
keyboard_symbols_contains(keysym_list* symbols, KeySym symbol)
{
    int i;
    for(i = 0; i < symbols->symbol_count; i++)
    {
	KeySym compare_symbol = *(symbols->symbols + i);
	if(compare_symbol == symbol)
	{
	    return 1;
	}
    }
    return 0;
}

static void
keyboard_map_virtual_key(keyboard_x11* keyboard, KeySym symbol, virtual_key key)
{
    int keycode_index;
    for(keycode_index = 0; keycode_index < keyboard->keycode_count; keycode_index++)
    {
	int keycode = (keyboard->keycode_min + keycode_index);

	keysym_list* symbols = (keyboard->map + keycode_index);
	if(keyboard_symbols_contains(symbols, symbol))
	{
	    *(keyboard->vkey_to_keycode + key) = keycode;
	    break;
	}
    }
}

static keyboard_x11
keyboard_x11_init(Display* display)
{
    keyboard_x11 input = (keyboard_x11){0};
    create_keycode_map(display, &input);

    int i;
    
    int alpha_count = (VKEY_Z - VKEY_A) + 1;
    for(i = 0; i < alpha_count; i++)
    {
	virtual_key key = (virtual_key)(VKEY_A + i);
	KeySym symbol = (KeySym)(XK_A + i);
	
	keyboard_map_virtual_key(&input, symbol, key);
    }

    keyboard_map_virtual_key(&input, XK_Escape, VKEY_ESCAPE);

    return input;
}

static void
keyboard_x11_reset(keyboard_x11* keyboard)
{
    int i;
    for(i = 0; i < MAX_KEYCODE_COUNT; i++)
    {
	keycode_state* state = (keyboard->state + i);
	state->pressed = 0;
	state->released = 0;
    }
}

static inline bool32
keyboard_x11_is_down(keyboard_x11* keyboard, virtual_key key)
{
    int keycode = *(keyboard->vkey_to_keycode + key);
    return (keyboard->state + keycode)->down;
}

static inline bool32
keyboard_x11_is_pressed(keyboard_x11* keyboard, virtual_key key)
{
    int keycode = *(keyboard->vkey_to_keycode + key);
    return (keyboard->state + keycode)->pressed;
}

static inline bool32
keyboard_x11_is_released(keyboard_x11* keyboard, virtual_key key)
{
    int keycode = *(keyboard->vkey_to_keycode + key);
    return (keyboard->state + keycode)->released;
}
