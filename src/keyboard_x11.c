// keyboard_x11.c

#include <X11/Xlib.h>
#include "keyboard_x11.h"

// TODO: Depend on platform for logging instead of stdio.
#include <stdio.h>

keycode_map
create_keycode_map(Display* display)
{
    keycode_map result = (keycode_map){0};
    
    int keycode_min, keycode_max;
    XDisplayKeycodes(display, &keycode_min, &keycode_max);

    int keycode_count = (keycode_max - keycode_min);
    if(keycode_count > MAX_KEYCODE_COUNT)
    {
	printf("warning: keycode_count(%d) > MAX_KEYCODE_COUNT(%d)\n", keycode_count, MAX_KEYCODE_COUNT);
	keycode_count = MAX_KEYCODE_COUNT;
	keycode_max = (keycode_min + keycode_count);
    }

    result.keycode_min = keycode_min;
    result.keycode_max = keycode_max;
    result.keycode_count = keycode_count;

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
	    // keycode = (keycode_min + keycode_index);
	    symbols = (result.symbol_list + keycode_index);

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
    
    return result;
}

int
keycode_is_symbol(keycode_map* keyboard_map, int keycode, KeySym symbol)
{
    int keycode_index = (keycode - keyboard_map->keycode_min);
    if(keycode_index < 0 || keycode_index >= keyboard_map->keycode_count)
    {
	printf("warning: keycode %d is outside valid range (%d - %d).",
	       keycode, keyboard_map->keycode_min, keyboard_map->keycode_max);
	return 0;
    }

    keysym_list* symbol_list = (keyboard_map->symbol_list + keycode_index);

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
