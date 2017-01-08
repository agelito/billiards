// main.c

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

    keyboard_input keyboard = keyboard_init(window_context.display);
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
	
	xinput2_mouse mouse_axis = xinput2_get_default_axis(&mouse_raw);
	mouse_apply_relative(&racera_state.mouse, mouse_axis.x, mouse_axis.y, mouse_axis.wheel);
	mouse_clamp_to_window(&racera_state.mouse, window_context.width, window_context.height);
	
	game_update_and_render(&racera_state);

	redraw_window(&window_context);

	keyboard_reset_state(&keyboard);
	xinput2_reset_axis_data(&mouse_raw);
	
	platform_sleep(1);
    }

    return 0;
}
