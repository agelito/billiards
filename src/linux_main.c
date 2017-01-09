// linux_main.c

#include "platform.h"
#include "racera.h"

#include "window_x11.h"
#include "keyboard_x11.h"

#include "mouse_x11.h"
#include "mouse_xi2.h"

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    char exe_dir[1024];
    int exe_dir_length = 1024;
    
    exe_dir_length = platform_executable_directory(exe_dir, exe_dir_length);
    platform_set_working_directory(exe_dir);
    
    window_x11 window_context = create_window(1280, 720, "racera");

    keyboard_x11 keyboard = keyboard_x11_init(window_context.display);
    xinput2 mouse_raw = xinput2_init(window_context.display);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    game_state racera_state = (game_state){0};

    while(handle_window_events(&window_context, &keyboard, &mouse_raw))
    {
	racera_state.screen_width = window_context.width;
	racera_state.screen_height = window_context.height;

	int key;
	for(key = 0; key < VKEY_COUNT; key++)
	{
	    *(racera_state.keyboard.down + key) = keyboard_x11_is_down(&keyboard, key);
	    *(racera_state.keyboard.pressed + key) = keyboard_x11_is_pressed(&keyboard, key);
	    *(racera_state.keyboard.released + key) = keyboard_x11_is_released(&keyboard, key);
	}
	
	xinput2_mouse mouse_axis = xinput2_get_default_axis(&mouse_raw);
	mouse_apply_relative(&racera_state.mouse, mouse_axis.x, mouse_axis.y, mouse_axis.wheel);
	mouse_clamp_to_window(&racera_state.mouse, window_context.width, window_context.height);
	
	game_update_and_render(&racera_state);
	if(racera_state.should_quit)
	{
	    destroy_window(&window_context);
	    break;
	}

	redraw_window(&window_context);

	keyboard_x11_reset(&keyboard);
	xinput2_reset_axis_data(&mouse_raw);
	
	platform_sleep(1);
    }

    return 0;
}
