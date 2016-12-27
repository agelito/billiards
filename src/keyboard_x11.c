// keyboard_x11.c

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "keyboard_x11.h"

// TODO: Depend on platform for logging instead of stdio.
#include <stdio.h>

static void
create_keycode_map(Display* display, keyboard_input* keyboard)
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

static int
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
keyboard_map_virtual_key(keyboard_input* keyboard, KeySym symbol, virtual_key key)
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

keyboard_input
keyboard_init(Display* display)
{
    keyboard_input input = (keyboard_input){0};
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

void
keyboard_reset_state(keyboard_input* keyboard)
{
    int i;
    for(i = 0; i < MAX_KEYCODE_COUNT; i++)
    {
	keycode_state* state = (keyboard->state + i);
	state->pressed = 0;
	state->released = 0;
    }
}

int
keycode_is_symbol(keyboard_input* keyboard, int keycode, KeySym symbol)
{
    int keycode_index = (keycode - keyboard->keycode_min);
    if(keycode_index < 0 || keycode_index >= keyboard->keycode_count)
    {
	printf("warning: keycode %d is outside valid range (%d - %d).",
	       keycode, keyboard->keycode_min, keyboard->keycode_max);
	return 0;
    }

    keysym_list* symbol_list = (keyboard->map + keycode_index);

    int symbol_match = 0;
    
    int symbol_index;
    for(symbol_index = 0; symbol_index < symbol_list->symbol_count; ++symbol_index)
    {
	KeySym compare_symbol = *(symbol_list->symbols + symbol_index);
	if(compare_symbol == symbol)
	{
	    symbol_match = 1;
	    break;
	}
    }

    return symbol_match;
}

int is_down(keyboard_input* keyboard, virtual_key key)
{
    int keycode = *(keyboard->vkey_to_keycode + key);
    return (keyboard->state + keycode)->down;
}

int is_pressed(keyboard_input* keyboard, virtual_key key)
{
    int keycode = *(keyboard->vkey_to_keycode + key);
    return (keyboard->state + keycode)->pressed;
}

int is_released(keyboard_input* keyboard, virtual_key key)
{
    int keycode = *(keyboard->vkey_to_keycode + key);
    return (keyboard->state + keycode)->released;
}
